#pragma once

#include <charconv>
#include <map>
#include <stdexcept>
#include <string_view>
#include <tuple>
#include <variant>
#include <vector>

/**
 * A library for encoding and decoding bencode data. Format expectations come
 * from https://wiki.theory.org/BitTorrentSpecification.
 */

namespace libbencode {

/**
 * A discriminated union representing the standard bencode types. The union is
 * parameterizable so that it can be used in fixed-point combinators to become a
 * recursive type.
 *
 * @tparam T A type that, if inherited from BencodeValue, creates a recursive
 *           type definition.
 */
template <typename T>
using BencodeValue = std::variant<int64_t, std::string, std::vector<T>,
                                  std::map<std::string, T>>;

/**
 * A generic type used to create a recursive bencode type.
 *
 * @tparm U A template template parameter used to set up a fixed-point
 *          combinator.
 */
template <template <typename> typename U>
class FixedPointTerm : public U<FixedPointTerm<U>> {
  using U<FixedPointTerm>::U;
};

// Complete the recursive type construction.
using BencodeCombinator = FixedPointTerm<BencodeValue>;
/** A list that can hold any of the standardized bencode types. */
using BencodeList = std::vector<BencodeCombinator>;
/** A map that can hold any of the standardized bencode types. */
using BencodeDict = std::map<std::string, BencodeCombinator>;

/**
 * A type holding a result of parsing a value out of bencode data. The second
 * value is the number of bytes read from the data to parse the returned value.
 */
template <typename ParsedType>
using ParseResult = std::tuple<ParsedType, uint64_t>;

/**
 * Parse an integer out of bencode data.
 *
 * @p bencode Bencode data. Let the bencode specification for well-formatted
 *            integers be the contract for this parameter.
 * @returns The parsed integer and the number of bytes, inclusive, from the
 *          beginning of the data used to parse the integer.
 * @throws std::runtime_error Thrown for any failure to parse an integer out of
 *                            the data.
 */
ParseResult<int64_t> DecodeInt(const std::string_view& bencode) {
  // Implementation detail: we check for basic formatting violations. We don't
  // check for cases such as i123b456e or int64_t overflow.

  if (bencode.size() < 3) {
    throw std::runtime_error("Not enough data to parse an int.");
  }
  if (bencode.substr(1, 2) == "-0") {
    throw std::runtime_error("Invalid '-0' found.");
  }
  if (bencode[0] != 'i') {
    throw std::runtime_error("First character was not 'i'.");
  }

  size_t e_idx = bencode.find('e');

  if (e_idx == bencode.npos) {
    throw std::runtime_error("No 'e' trailer found in data.");
  }

  int64_t i = 0;
  auto [ptr, err] = std::from_chars(&bencode[1], &bencode[e_idx], i);

  if (err != std::errc()) {
    throw std::runtime_error("Failed to interpret int from chars.");
  }

  return {i, e_idx + 1};
}

/**
 * Parse a string out of bencode data.
 *
 * @p bencode Bencode data. Let the bencode specification for well-formatted
 *            strings be the contract for this parameter.
 * @returns The parsed string and the number of bytes, inclusive, from the
 *          beginning of the data used to parse the string.
 * @throws std::runtime_error Thrown for any failure to parse a string out of
 *                            the data.
 */
ParseResult<std::string> DecodeStr(const std::string_view& bencode) {
  if (!bencode.size()) {
    throw std::runtime_error("Not enough data to parse a string.");
  }

  if (!std::isdigit(bencode[0])) {
    throw std::runtime_error("Failed to parse string length.");
  }

  size_t colon_idx = bencode.find(':');

  if (colon_idx == bencode.npos) {
    throw std::runtime_error("No ':' delimiter found in data.");
  }

  size_t str_len = 0;
  auto [ptr, err] = std::from_chars(&bencode[0], &bencode[colon_idx], str_len);

  if (err != std::errc()) {
    throw std::runtime_error("Failed to parse string length.");
  }

  const size_t total_bytes_for_str = colon_idx + 1 + str_len;

  if (bencode.size() < total_bytes_for_str) {
    throw std::runtime_error("Not enough data to parse a string.");
  }

  std::string str{str_len ? bencode.substr(colon_idx + 1, str_len) : ""};

  return {str, total_bytes_for_str};
}

// Forward-declare a helper function.
ParseResult<BencodeCombinator> DecodeValue(const std::string_view& bencode);

/**
 * Parse a list out of bencode data. At this time, only lists of integers and
 * strings are supported.
 *
 * @p bencode Bencode data. Let the bencode specification for well-formatted
 *            lists be the contract for this parameter.
 * @returns The parsed list and the number of bytes, inclusive, from the
 *          beginning of the data used to parse the list.
 * @throws std::runtime_error Thrown for any failure to parse a list out of
 *                            the data.
 */
ParseResult<BencodeList> DecodeList(const std::string_view& bencode) {
  if (!bencode.size()) {
    throw std::runtime_error("Not enough data to parse a list.");
  }

  if (bencode[0] != 'l') {
    throw std::runtime_error("First character was not 'l'.");
  }

  BencodeList list;
  size_t bencode_idx = 1; // Skip the leading 'l'.
  while (bencode[bencode_idx] != 'e') {
    auto [val, num_bytes] = DecodeValue(bencode.substr(bencode_idx));

    // Extract a value and emplace it in list.
    // TODO(orphen) Find a way to share this general logic with DecodeDict.
    if (const auto* v = std::get_if<int64_t>(&val)) {
      list.emplace_back(*v);
    } else if (const auto* v = std::get_if<std::string>(&val)) {
      list.emplace_back(*v);
    } else {
      throw std::runtime_error("Unknown value type in bencode.");
    }

    bencode_idx += num_bytes;
  }

  return {list, bencode_idx + 1};
}

/**
 * Parse a dictionary out of bencode data. At this time, only integers and
 * strings are supported as values.
 *
 * @p bencode Bencode data. Let the bencode specification for well-formatted
 *            dictionaries be the contract for this parameter.
 * @returns The parsed dictionary and the number of bytes, inclusive, from the
 *          beginning of the data used to parse the dictionary.
 * @throws std::runtime_error Thrown for any failure to parse a dictionary out
 *                            of the data.
 */
ParseResult<BencodeDict> DecodeDict(const std::string_view& bencode) {
  if (!bencode.size()) {
    throw std::runtime_error("Not enough data to parse a dictionary.");
  }

  if (bencode[0] != 'd') {
    throw std::runtime_error("First character was not 'd'.");
  }

  BencodeDict dict;
  size_t bencode_idx = 1; // Skip the leading 'd'.

  while (bencode[bencode_idx] != 'e') {
    auto [key, num_bytes_key] = DecodeStr(bencode.substr(bencode_idx));
    bencode_idx += num_bytes_key;

    auto [val, num_bytes] = DecodeValue(bencode.substr(bencode_idx));

    // Extract a value and emplace it in the dictionary.
    // TODO(orphen) Find a way to share this general logic with DecodeList.
    if (const auto* v = std::get_if<int64_t>(&val)) {
      dict.emplace(key, *v);
    } else if (const auto* v = std::get_if<std::string>(&val)) {
      dict.emplace(key, *v);
    } else {
      throw std::runtime_error("Unknow value type in bencode.");
    }

    bencode_idx += num_bytes;
  }

  return {dict, bencode_idx + 1};
}

/**
 * A helper function that calls one of the other Decode* functions based on the
 * bencode type intuited from the first character in @p bencode.
 *
 * @p bencode Bencode data. Let the bencode specification for well-formatted
 *            data be the contract for this parameter.
 * @returns A variant holding the value of one of the fundamental bencode types.
 * @throws std::runtime_error Thrown if a bencode type cannot be intuited.
 */
ParseResult<BencodeCombinator> DecodeValue(const std::string_view& bencode) {
  if (bencode.empty()) {
    throw std::runtime_error("Not enough data to intuit bencode type.");
  }

  const auto& indicator = bencode[0];
  if (indicator == 'i') {
    return DecodeInt(bencode);
  } else if (indicator == 'l') {
    return DecodeList(bencode);
  } else if (indicator == 'd') {
    return DecodeDict(bencode);
  } else if (std::isdigit(indicator)) {
    return DecodeStr(bencode);
  }

  throw std::runtime_error("Unsupported bencode type.");
}

}; // namespace libbencode
