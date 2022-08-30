#include <cassert>
#include <string>
#include <tuple>

#include "lib/assertm.h"
#include "lib/libbencode.h"

int main(void) {
  // Test input that satisfies the `DecodeList` contract.

  using libbencode::BencodeDict;
  using libbencode::BencodeList;
  using libbencode::DecodeList;

  // Empty list.
  std::string test_case = "le";
  auto [list, num_bytes] = DecodeList(test_case);
  assertm(0 == list.size() && 2 == num_bytes, "Parse failed.");

  // List with a single integer.
  test_case = "li1ee";
  std::tie(list, num_bytes) = DecodeList(test_case);
  assertm(1 == list.size() && 5 == num_bytes, "Parse failed.");
  assertm(*std::get_if<int64_t>(&list[0]) == 1,
          "Unexpected list element value.");

  // List with a single string.
  test_case = "l4:abcde";
  std::tie(list, num_bytes) = DecodeList(test_case);
  assertm(1 == list.size() && 8 == num_bytes, "Parse failed.");
  assertm(*std::get_if<std::string>(&list[0]) == "abcd",
          "Unexpected list element value.");

  // List with an integer and a string.
  test_case = "li1e4:abcde";
  std::tie(list, num_bytes) = DecodeList(test_case);
  assertm(2 == list.size() && 11 == num_bytes, "Parse failed.");
  assertm(*std::get_if<int64_t>(&list[0]) == 1,
          "Unexpected list element value.");
  assertm(*std::get_if<std::string>(&list[1]) == "abcd",
          "Unexpected list element value.");

  // List with an integer and a string and extra characters in bencode.
  test_case = "li1e4:abcdeyyy";
  std::tie(list, num_bytes) = DecodeList(test_case);
  assertm(2 == list.size() && 11 == num_bytes, "Parse failed.");
  assertm(*std::get_if<int64_t>(&list[0]) == 1,
          "Unexpected list element value.");
  assertm(*std::get_if<std::string>(&list[1]) == "abcd",
          "Unexpected list element value.");

  // List with a list.
  test_case = "lli1eee";
  std::tie(list, num_bytes) = DecodeList(test_case);
  assertm(1 == list.size() && 7 == num_bytes, "Parse failed.");
  const BencodeList* sub_list = nullptr;
  assertm(sub_list = std::get_if<BencodeList>(&list[0]), "Parse failed.");
  assertm(*std::get_if<int64_t>(&(*sub_list)[0]) == 1, "Parse failed.");

  // List with a dict.
  test_case = "ld1:a1:bee";
  std::tie(list, num_bytes) = DecodeList(test_case);
  assertm(1 == list.size() && 10 == num_bytes, "Parse failed.");
  BencodeDict* dict = nullptr;
  assertm(dict = std::get_if<BencodeDict>(&list[0]), "Parse failed.");
  assertm(*std::get_if<std::string>(&(*dict)["a"]) == "b", "Parse failed.");

  return 0;
}
