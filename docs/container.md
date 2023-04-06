# Container build and offline smoke test

Initialize the pinned submodule first, then:

```sh
docker build -t dc-bot:local .
docker run --rm --network none --read-only --cap-drop ALL \
  dc-bot:local --check-config --settings /app/bot.conf
docker run --rm --network none dc-bot:local --version
```

The multi-stage build runs the complete offline check suite on Linux. The final
image contains the bot, default non-secret settings, libcurl and CA certificates,
not a compiler or credentials. It runs as numeric user/group `10001:10001`.
`.dockerignore` allowlists build inputs and excludes Git metadata, copied object
files, local tokens, logs and old source scratch copies.

For a **live** connection, pass the token from an already-populated environment
using `-e DISCORD_TOKEN` (no value on the command line), or use a private mode-600
`--env-file` outside the repository. Do not use Docker `ARG`/`ENV` instructions
for real credentials. Mount a customized settings file read-only and keep the
network enabled only for live operation. No inbound port mapping is required.

Do not label `--check-config` a live health check: it validates local settings
only. Running without `--help`, `--version` or `--check-config` performs network
I/O during Concord initialization. Container shutdown currently relies on the
runtime's normal process termination; no asynchronous signal handler calls into
Concord because its shutdown API is not documented as signal-safe.
