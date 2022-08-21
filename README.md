# libbencode

A library for encoding and decoding
[bencode](https://en.wikipedia.org/wiki/Bencode) data. Implementation follows
the description provided in the
[unofficial Bittorrent specification](https://wiki.theory.org/BitTorrentSpecification)
document.

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
