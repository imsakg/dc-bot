# Commands

The default prefix is `!`. Command names are **case-sensitive** lower-case ASCII
words; `!pingpong` does not match `!ping`. A space or tab separates arguments.
A configurable prefix can contain 1–8 printable ASCII characters.

## Information

- `!help` — list every command name using the compiled registry.
- `!about` — bot version and implementation stack.
- `!ping` → `pong`; `!pong` → `ping`. These do not measure network latency.
- `!uptime` — process lifetime in days, hours, minutes and seconds; reconnects
  do not reset it. Backward wall-clock changes clamp the result to zero.
- `!stats` — human messages observed, replies accepted for queueing, and ready
  events. These are local process counters, not Discord delivery receipts.
- `!userinfo` — invoking user's name and ID, without exposing private fields.
- `!serverinfo` — current guild and channel IDs; a friendly explanation in DMs.
- `!avatar` — invoking user's validated CDN avatar URL; animated hashes use GIF.
  Both legacy discriminator-based and newer ID-based default avatars work.
- `!snowflake 175928847299117063` — creation time as a native Discord timestamp.
  IDs must be positive decimal uint64 values; signs and overflow are rejected.

## Text

- `!echo hello` — repeat text literally (`%s` is not interpreted).
- `!upper hello` — ASCII upper-case conversion.
- `!lower HELLO` — ASCII lower-case conversion.

Case conversion preserves non-ASCII UTF-8 bytes; it is not Unicode case folding.
Empty text arguments return usage. Replies over 1,900 UTF-8 **bytes** are rejected
rather than truncated. Markdown is not escaped, but all reply types explicitly
suppress user, role and everyone notifications, including reply-author pings.

## Small games

- `!choose tea | coffee | water` — choose among 2–16 non-empty options. Spaces
  around options are trimmed; literal pipe characters cannot be escaped.
- `!roll` — roll 1–6; `!roll 20` rolls 1–20. Sides range from 2 to 1,000,000.
- `!coin` — heads or tails.
- `!eightball Will it build?` — one of six answers; a question is required.

Randomness is for entertainment, not security, gambling or fair-money draws.
Unknown names are silently ignored. All recognized commands share the same
per-user/per-channel cooldown; malformed arguments still consume that slot.
See [configuration](configuration.md) for limits and privileged intents.
