# libbencode

A library for decoding [bencode](https://en.wikipedia.org/wiki/Bencode) data.
Implementation follows the description provided in the
[unofficial Bittorrent specification](https://wiki.theory.org/BitTorrentSpecification).

This project is an experiment in building recursive types with the C++ type
system. It is not intended to be performant, elegant, or easy to use.

**License:** MIT

## Usage

The library is header-only, so you only have to include it where you'd like to
use it. For documentation, see the `lib/libbencode.h` source file.

The library provides 4 main functions in the `libbencode` namespace.

```cpp
ParseResult<int64_t> DecodeInt(const std::string_view& bencode);
ParseResult<std::string> DecodeStr(const std::string_view& bencode);
ParseResult<BencodeList> DecodeList(const std::string_view& bencode);
ParseResult<BencodeDict> DecodeDict(const std::string_view& bencode);
```

The `bencode` parameter is an ASCII-encoded bencode string. `ParseResult` is a
pair of a bencode value and the number of bytes read from `bencode` to parse the
bencode value.

`BencodeList` and `BencodeDict` are recursive types that can contain
combinations of instances of `int64_t`, `std::string`, `BencodeList`, and
`BencodeDict`. The list and dictionary elements are therefore instances of
`std::variant`.

This library is easiest to use when dealing with bencode of some anticipated
structure, such as a [torrent file](https://en.wikipedia.org/wiki/Torrent_file).
This way, you can attempt to parse the exact types you need instead of having to
try all types to see what kind of value any variant holds.

Consider this simple example of an integer, a list that contains a string, and
another list containing an integer.

```cpp
using namespace libbencode;

std::string bencode = "i1el4:abcdli1eee";
std::string_view bencode_v(bencode);

// Get the first integer.
size_t bencode_idx = 0;
auto [first_int, num_bytes] = DecodeInt(bencode_v);
bencode_idx += num_bytes;

// Now parse the list.
BencodeList list;
std::tie(list, num_bytes) = DecodeList(bencode_v.substr(bencode_idx));

// Read the string and list out of the list.
std::string str = std::get<std::string>(list[0]);
BencodeList nested_list = std::get<BencodeList>(list[1]);
```

## Development

As the library is header-only, it does not need to be built. However, the tests
need to be compiled. You can use the tests or other compiled programs to debug
the library.

**macOS:**

```
# Building
bazel build --config=macos //...

# If you need debug symbols
bazel build --config=macos -c dbg --spawn_strategy=local //...

# Testing
bazel test --config=macos //...
```

## TODOs

1. Replace exceptions with `std::expected` when we have C++23.
