#include <cassert>
#include <string>
#include <tuple>

#include "libbencode/assertm.h"
#include "libbencode/libbencode.h"

int main(void) {
  // Test input that satisfies the `DecodeInt` contract.

  using libbencode::DecodeInt;

  std::string test_case = "i0e";
  auto [integer, num_bytes] = DecodeInt(test_case);
  assertm(0 == integer && num_bytes == test_case.size(), "Parse failed.");

  test_case = "i1e";
  std::tie(integer, num_bytes) = DecodeInt(test_case);
  assertm(1 == integer && num_bytes == test_case.size(), "Parse failed.");

  test_case = "i123e";
  std::tie(integer, num_bytes) = DecodeInt(test_case);
  assertm(123 == integer && num_bytes == test_case.size(), "Parse failed.");

  test_case = "i-1e";
  std::tie(integer, num_bytes) = DecodeInt(test_case);
  assertm(-1 == integer && num_bytes == test_case.size(), "Parse failed.");

  test_case = "i-123e";
  std::tie(integer, num_bytes) = DecodeInt(test_case);
  assertm(-123 == integer && num_bytes == test_case.size(), "Parse failed.");

  test_case = "i123efoo";
  std::tie(integer, num_bytes) = DecodeInt(test_case);
  assertm(123 == integer && 5 == num_bytes, "Parse failed.");

  test_case = "i-123efoo";
  std::tie(integer, num_bytes) = DecodeInt(test_case);
  assertm(-123 == integer && 6 == num_bytes, "Parse failed.");

  return 0;
}
