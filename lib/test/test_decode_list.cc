#include <cassert>
#include <string>
#include <tuple>

#include "lib/assertm.h"
#include "lib/libbencode.h"

int main(void) {
  // Test input that satisfies the `DecodeList` contract.

  using libbencode::DecodeList;

  std::string test_case = "le";
  auto [list, num_bytes] = DecodeList(test_case);
  assertm(0 == list.size() && 2 == num_bytes, "Parse failed.");

  test_case = "li1ee";
  std::tie(list, num_bytes) = DecodeList(test_case);
  assertm(1 == list.size() && 5 == num_bytes, "Parse failed.");
  assertm(*std::get_if<int64_t>(&list[0]) == 1,
          "Unexpected list element value.");

  test_case = "l4:abcde";
  std::tie(list, num_bytes) = DecodeList(test_case);
  assertm(1 == list.size() && 8 == num_bytes, "Parse failed.");
  assertm(*std::get_if<std::string>(&list[0]) == "abcd",
          "Unexpected list element value.");

  test_case = "li1e4:abcde";
  std::tie(list, num_bytes) = DecodeList(test_case);
  assertm(2 == list.size() && 11 == num_bytes, "Parse failed.");
  assertm(*std::get_if<int64_t>(&list[0]) == 1,
          "Unexpected list element value.");
  assertm(*std::get_if<std::string>(&list[1]) == "abcd",
          "Unexpected list element value.");

  test_case = "li1e4:abcdeyyy";
  std::tie(list, num_bytes) = DecodeList(test_case);
  assertm(2 == list.size() && 11 == num_bytes, "Parse failed.");
  assertm(*std::get_if<int64_t>(&list[0]) == 1,
          "Unexpected list element value.");
  assertm(*std::get_if<std::string>(&list[1]) == "abcd",
          "Unexpected list element value.");

  return 0;
}
