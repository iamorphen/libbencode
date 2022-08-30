# libbencode

A library for encoding and decoding
[bencode](https://en.wikipedia.org/wiki/Bencode) data. Implementation follows
the description provided in the
[unofficial Bittorrent specification](https://wiki.theory.org/BitTorrentSpecification).

This project is an experiment with using templates and the C++ type system to
create recursive data types. It is not intended to be performant, elegant, or
easy to use.

## Building

**macOS:**

```
bazel build --config=macos //...
```

If you need debug symbols, then

```
bazel build --config=macos -c dbg --spawn_strategy=local //...
```

## Testing

**macOS:**

```
bazel test --config=macos //...
```

# TODOs

1. Replace exceptions with `std::expected` when we have C++23.
