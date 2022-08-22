#pragma once

#include <charconv>
#include <stdexcept>
#include <string_view>
#include <tuple>

/**
 * A library for encoding and decoding bencode data. Format expectations come
 * from https://wiki.theory.org/BitTorrentSpecification.
 */

namespace libbencode {

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

  if (bencode[0] == '-') {
    throw std::runtime_error("String length cannot be negative.");
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

}; // namespace libbencode
