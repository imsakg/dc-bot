# dc-bot

A small C Discord bot built with [Concord v2.2.0](https://github.com/Cogmasters/concord).

## Build

```sh
git clone --recurse-submodules https://github.com/imsakg/dc-bot.git
cd dc-bot
# For an existing clone:
git submodule update --init --recursive
make
```

Use a C99 compiler, GNU Make and libcurl development headers. On Debian:
`apt-get install build-essential libcurl4-openssl-dev ca-certificates`.
On macOS, Xcode Command Line Tools provide the compiler and libcurl.

The checked-out Concord revision is pinned by the submodule. Never update it
implicitly during a build. See [the development plan](docs/development-plan.md).
