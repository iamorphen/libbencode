#include <cassert>
#include <string>
#include <tuple>

#include "libbencode/assertm.h"
#include "libbencode/libbencode.h"

int main(void) {
  // Test input that satisfies the `DecodeDict` contract.

  using libbencode::BencodeDict;
  using libbencode::BencodeList;
  using libbencode::DecodeDict;

  // Empty dict.
  std::string test_case = "de";
  auto [dict, num_bytes] = DecodeDict(test_case);
  assertm(0 == dict.size() && 2 == num_bytes, "Parse failed.");

  // Dict with an int entry.
  test_case = "d1:ai0ee";
  std::tie(dict, num_bytes) = DecodeDict(test_case);
  assertm(1 == dict.size() && 8 == num_bytes, "Parse failed.");
  assertm(0 == *std::get_if<int64_t>(&dict["a"]), "Unexpected map value.");

  // Dict with a string entry.
  test_case = "d2:bb4:abcde";
  std::tie(dict, num_bytes) = DecodeDict(test_case);
  assertm(1 == dict.size() && 12 == num_bytes, "Parse failed.");
  assertm(*std::get_if<std::string>(&dict["bb"]) == "abcd",
          "Unexpected map value.");

  // Dict with an int entry and a string entry.
  test_case = "d1:ai0e2:bb4:abcde";
  std::tie(dict, num_bytes) = DecodeDict(test_case);
  assertm(2 == dict.size() && 18 == num_bytes, "Parse failed.");
  assertm(0 == *std::get_if<int64_t>(&dict["a"]), "Unexpected map value.");
  assertm(*std::get_if<std::string>(&dict["bb"]) == "abcd",
          "Unexpected map value.");

  // Dict with an int entry and a string entry and then extra bencode.
  test_case = "d1:ai0e2:bb4:abcdeyyy";
  std::tie(dict, num_bytes) = DecodeDict(test_case);
  assertm(2 == dict.size() && 18 == num_bytes, "Parse failed.");
  assertm(0 == *std::get_if<int64_t>(&dict["a"]), "Unexpected map value.");
  assertm(*std::get_if<std::string>(&dict["bb"]) == "abcd",
          "Unexpected map value.");

  // Dict with a list.
  test_case = "d1:ali1eee";
  std::tie(dict, num_bytes) = DecodeDict(test_case);
  assertm(1 == dict.size() && 10 == num_bytes, "Parse failed.");
  const BencodeList* list = nullptr;
  assertm(list = std::get_if<BencodeList>(&dict["a"]), "Parse failed.");
  assertm(*std::get_if<int64_t>(&(*list)[0]) == 1, "Parse failed.");

  // Dict with a dict.
  test_case = "d1:ad1:bi1eee";
  std::tie(dict, num_bytes) = DecodeDict(test_case);
  assertm(1 == dict.size() && 13 == num_bytes, "Parse failed.");
  BencodeDict* sub_dict = nullptr;
  assertm(sub_dict = std::get_if<BencodeDict>(&dict["a"]), "Parse failed.");
  assertm(*std::get_if<int64_t>(&(*sub_dict)["b"]) == 1, "Parse failed.");

  return 0;
}
