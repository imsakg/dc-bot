# Bot expansion implementation plan

> **For Hermes:** Implement one coherent, tested change per commit. Use
> test-driven-development for new behavior; keep network access out of tests.

**Goal:** Turn the ping/pong sample into a useful, bounded prefix-command bot.

**Architecture:** A portable C99 core owns parsing and response formatting.
Concord adapters own Discord events and transport. A strict optional key=value
file holds non-secret bot settings; Concord JSON or an environment variable
supplies credentials. Tests exercise the core without Discord.

**Tech stack:** C99, GNU Make, Concord 2.2.0, libcurl; Python 3 for CLI tests.

## Ordered slices

1. Repair Makefile build/run separation; verify a fresh native build.
2. Add `inc/bot_core.h`, `src/bot_core.c`, `tests/test_core.c` and `make test`.
   Observe RED for ping/pong, then implement GREEN.
3. Add strict prefix parsing, exact command boundaries and length limits with
   table-driven tests before each implementation.
4. Add help, about, echo, text transformations, bounded choice/dice/coin/8ball
   commands. Inject randomness so tests can assert concrete results.
5. Add strict integer/snowflake helpers and atomic configuration parsing in
   `src/bot_config.c`; test invalid keys, values, overflow and defaults.
6. Wire startup, CLI validation and environment credentials in `src/dc_bot.c`.
   Test CLI modes without connecting to Discord.
7. Add transport adapters in `src/events.c`, with offline callbacks testing
   bot/webhook guards, mention suppression, guild events and reply failures.
8. Add uptime, stats, user/server/avatar commands and bounded rate limiting.
9. Document commands, settings, intents, permissions and safe operations.
10. Add sanitizer/property/header checks, a container recipe and CI.
11. Independently review; run native build, offline suite and sanitizers.

For each behavioral slice: add a focused failing assertion, run it, implement
only that contract, run the full offline suite and commit specific paths.
Avoid token logging, privileged moderation commands, shell execution and
unbounded responses. Keep ignored local credentials and pre-existing files.

## Acceptance and history

The deliverable is 60 non-empty commits reachable from `main`, a successful
native `make`, offline tests and documented configuration. The requested
2022-06 through 2023-04 weekday/evening dates are a **reconstructed development
timeline**, not evidence these additions were originally authored then.
Original history is preserved locally at `backup/pre-history-expansion`.
Do not push this rewritten history.
