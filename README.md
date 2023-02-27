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

## Run

Supply `DISCORD_TOKEN` through your process manager or a private environment.
Never paste it into source control, command-line arguments, or public logs.

```sh
./build/dc_bot --help
./build/dc_bot --version
./build/dc_bot --check-config --settings bot.conf.example
# After securely setting DISCORD_TOKEN in the process environment:
./build/dc_bot --settings bot.conf.example
```

Without the environment token, `--config config.json` selects Concord's private
credential/logging JSON. Copy `config.example.json` and replace its placeholder
locally; the real file is ignored. Never commit it.

- [Commands](docs/commands.md)
- [Settings, permissions and safety](docs/configuration.md)

`make` only builds. `make run` starts a live Discord connection deliberately.
`make test` runs offline C tests; `make check` also builds and checks CLI modes.

