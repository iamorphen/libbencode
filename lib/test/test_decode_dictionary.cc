#include <cassert>
#include <string>
#include <tuple>

#include "lib/assertm.h"
#include "lib/libbencode.h"

int main(void) {
  // Test input that satisfies the `DecodeDict` contract.

  using libbencode::DecodeDict;

  std::string test_case = "de";
  auto [dict, num_bytes] = DecodeDict(test_case);
  assertm(0 == dict.size() && 2 == num_bytes, "Parse failed.");

  test_case = "d1:ai0ee";
  std::tie(dict, num_bytes) = DecodeDict(test_case);
  assertm(1 == dict.size() && 8 == num_bytes, "Parse failed.");
  assertm(0 == *std::get_if<int64_t>(&dict["a"]), "Unexpected map value.");

  test_case = "d2:bb4:abcde";
  std::tie(dict, num_bytes) = DecodeDict(test_case);
  assertm(1 == dict.size() && 12 == num_bytes, "Parse failed.");
  assertm(*std::get_if<std::string>(&dict["bb"]) == "abcd",
          "Unexpected map value.");

  test_case = "d1:ai0e2:bb4:abcde";
  std::tie(dict, num_bytes) = DecodeDict(test_case);
  assertm(2 == dict.size() && 18 == num_bytes, "Parse failed.");
  assertm(0 == *std::get_if<int64_t>(&dict["a"]), "Unexpected map value.");
  assertm(*std::get_if<std::string>(&dict["bb"]) == "abcd",
          "Unexpected map value.");

  test_case = "d1:ai0e2:bb4:abcdeyyy";
  std::tie(dict, num_bytes) = DecodeDict(test_case);
  assertm(2 == dict.size() && 18 == num_bytes, "Parse failed.");
  assertm(0 == *std::get_if<int64_t>(&dict["a"]), "Unexpected map value.");
  assertm(*std::get_if<std::string>(&dict["bb"]) == "abcd",
          "Unexpected map value.");

  return 0;
}
