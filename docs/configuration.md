# Settings and Discord setup

## Two separate configuration surfaces

`DISCORD_TOKEN` is preferred for credentials. If present, including an empty
value, it takes precedence over the JSON file. Empty values, the sample token,
spaces, expansion markers and invalid characters fail before initialization.
Only ASCII letters, digits, dots, underscores and hyphens are accepted, with
a maximum length of 4,094 bytes. This is syntax checking, not authentication.
In this mode Concord uses its quiet default logger, not the JSON logging block.

Alternatively, `--config /private/config.json` loads Concord's JSON. With no
option it uses `config.json` in the working directory. The historical positional
`dc_bot config.json` form also works. Keep the file owner-readable only and
HTTP logging disabled. Never add bot tokens to build arguments or image layers.
The sample `YOUR-BOT-TOKEN` is a placeholder, never a usable credential.

`--settings bot.conf` is an optional **non-secret**, flat key=value file:

```ini
prefix=!
cooldown_seconds=2
welcome_guild=0
welcome_channel=0
```

- `prefix`: 1–8 printable ASCII characters, no whitespace. Default `!`.
- `cooldown_seconds`: decimal 0–3,600 inclusive; default 2. Zero disables it.
- `welcome_guild`, `welcome_channel`: both zero to disable join/leave messages,
  or both positive uint64 IDs to enable them for exactly that guild/channel.

Blank lines and whole-line `#` comments are allowed. Leading/trailing whitespace
and CRLF are trimmed. Inline comments and quoting are not supported. Empty,
unknown, duplicate, oversized or malformed settings are errors. Values are not
printed in diagnostics. A failed file load leaves the previous settings intact.
Settings load once at startup; there is no in-chat reload or token-management
command. The JSON `default_prefix` is not used by our message-create parser;
put the prefix in `--settings` instead.

Validate the settings without credentials or network activity:

```sh
./build/dc_bot --check-config --settings bot.conf.example
```

This validates **only bot settings**, not the JSON credential file or Discord
permissions. `--help` and `--version` also exit before initializing Concord.
Starting normally does network I/O during Concord client initialization itself,
even before the gateway loop starts. Bad remote credentials may cause Concord
2.2.0 to abort; supervise restarts and fix credentials rather than looping blindly.

## Discord Developer Portal

Create a bot application, keep its token private, and invite it using the `bot`
OAuth2 scope. The bot needs **View Channel** and **Send Messages** in channels
where it should respond; in threads also permit **Send Messages in Threads**.
It does not need Administrator, moderation, role-management or webhook rights.

Enable the privileged **Message Content Intent** for normal guild prefix
commands. The client requests guilds, guild messages, direct messages and message
content. When welcome messages are configured, it additionally requests the
privileged **Server Members Intent**, which must also be enabled in the portal.
Join and departure notices use the same configured channel and ignore bot users.
Do not enable member notices unless public join/leave announcements are desired.

## Operational limits

- Bots, webhooks, system users and non-normal/non-reply message events are ignored.
- The cooldown tracks 64 recent user/channel pairs in fixed memory. Oldest entries
  are evicted on saturation; this is a spam brake, not an abuse-proof quota system.
- Unknown names do not consume a cooldown; recognized commands with bad arguments do.
- Backward clock movement resets the matching cooldown rather than locking it out.
- Replies explicitly disable mention parsing. Echoed markdown still renders.
- Counters are process-local and disappear on restart. A queued reply is not proof
  of delivery; Discord REST rate limiting remains Concord's responsibility.
- No user input is executed as shell commands, file paths or format strings.

Historical commits originally tracked `config.json`. Removing it from the current
tree does not erase old Git objects: rotate any credential that was ever exposed
before sharing history. This repository's expanded timeline is reconstructed, as
stated in the [development plan](development-plan.md); do not treat dates as audit
proof of original authorship.
