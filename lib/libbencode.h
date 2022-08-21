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

}; // namespace libbencode
