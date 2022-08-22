#include <cassert>
#include <string>
#include <tuple>

#include "lib/assertm.h"
#include "lib/libbencode.h"

int main(void) {
  // Test input that satisfies the `DecodeStr` contract.

  using libbencode::DecodeStr;

  std::string test_case = "0:";
  auto [str, num_bytes] = DecodeStr(test_case);
  assertm(str == "" && num_bytes == test_case.size(), "Parse failed.");

  test_case = "1:a";
  std::tie(str, num_bytes) = DecodeStr(test_case);
  assertm(str == "a" && 3 == num_bytes, "Parse failed.");

  test_case = "10:abcdefghij";
  std::tie(str, num_bytes) = DecodeStr(test_case);
  assertm(str == "abcdefghij" && 13 == num_bytes, "Parse failed.");

  // Test a case where bytes follow the string to parse.
  test_case = "5:123456";
  std::tie(str, num_bytes) = DecodeStr(test_case);
  assertm(str == "12345" && 7 == num_bytes, "Parse failed.");

  return 0;
}
