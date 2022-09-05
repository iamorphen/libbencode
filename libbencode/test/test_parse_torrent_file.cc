#include <array>
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "tools/cpp/runfiles/runfiles.h"

#include "libbencode/assertm.h"
#include "libbencode/libbencode.h"

template <typename... Bytes>
std::array<std::byte, sizeof...(Bytes)> make_byte_array(Bytes... bytes) {
  return {std::byte(bytes)...};
}

std::string get_runfile(const std::string& runfile_rel_path) {
  using bazel::tools::cpp::runfiles::Runfiles;
  std::string runfiles_error;
  std::unique_ptr<Runfiles> runfiles(Runfiles::CreateForTest(&runfiles_error));

  assertm(runfiles, runfiles_error);

  return runfiles->Rlocation(runfile_rel_path);
}

std::string read_file(const std::string& pathname) {
  std::ifstream f(pathname);
  assertm(f.is_open(), "Failed to open file for reading.");
  std::stringstream file_contents;
  file_contents << f.rdbuf();
  return file_contents.str();
}

int main() {
  using libbencode::BencodeDict;
  using libbencode::BencodeList;
  using libbencode::DecodeDict;

  std::string torrent_file_path =
      get_runfile("libbencode/libbencode/test/data/"
                  "ubuntu-22.04.1-desktop-amd64.iso.torrent");
  std::string torrent_bencode = read_file(torrent_file_path);

  auto [torrent_info, _] = DecodeDict(torrent_bencode);
  assertm(std::get<std::string>(torrent_info["announce"]) ==
              std::string("https://torrent.ubuntu.com/announce"),
          "'announce' did not match expectations.");

  BencodeList bencode_list =
      std::get<BencodeList>(torrent_info["announce-list"]);
  BencodeList sublist_1 = std::get<BencodeList>(bencode_list[0]);
  assertm(std::get<std::string>(sublist_1[0]) ==
              std::string("https://torrent.ubuntu.com/announce"),
          "Sublist 1 from 'announce-list' did not match expectations.");
  BencodeList sublist_2 = std::get<BencodeList>(bencode_list[1]);
  assertm(std::get<std::string>(sublist_2[0]) ==
              std::string("https://ipv6.torrent.ubuntu.com/announce"),
          "Sublist 2 from 'announce-list' did not match expectations.");

  assertm(std::get<std::string>(torrent_info["comment"]) ==
              std::string("Ubuntu CD releases.ubuntu.com"),
          "'comment' did not match expectations.");
  assertm(std::get<std::string>(torrent_info["created by"]) ==
              std::string("mktorrent 1.1"),
          "'created by' did not match expectations.");
  assertm(std::get<int64_t>(torrent_info["creation date"]) == 1660215352,
          "'creation date' did not match expectations.");

  BencodeDict info_dict = std::get<BencodeDict>(torrent_info["info"]);

  assertm(std::get<int64_t>(info_dict["length"]) == 3826831360,
          "info_dict['length'] did not match expectations.");
  assertm(std::get<std::string>(info_dict["name"]) ==
              std::string("ubuntu-22.04.1-desktop-amd64.iso"),
          "info_dict['name'] did not match expectations.");
  assertm(std::get<int64_t>(info_dict["piece length"]) == 262144,
          "info_dict['piece length'] did not match expectations.");

  // "pieces" is a string of concatenated SHA-1 hashes. SHA-1 hashes are 20
  // bytes in length, so the length of "pieces" must be divisible by 20.
  std::string pieces = std::get<std::string>(info_dict["pieces"]);
  assertm(pieces.size() % 20 == 0,
          "info_dict['pieces'] did not match expectations.");

  // Check the first 20-byte hash.
  auto hash = make_byte_array(0x56, 0x7b, 0x9b, 0x5c, 0x3d, 0x06, 0x17, 0xb2,
                              0xda, 0xab, 0x0c, 0xe8, 0x88, 0xed, 0x2b, 0x9a,
                              0xc2, 0x33, 0x02, 0xfc);
  for (auto i = 0; i < hash.size(); i++) {
    assertm(std::byte(static_cast<uint8_t>(pieces[i])) == hash[i],
            "Hash value mismatch.");
  }

  return 0;
}
