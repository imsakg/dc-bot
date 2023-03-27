# Development and verification

```sh
make                  # compile only, never connect
make test             # core, settings, stubbed Discord events and property tests
make check            # above, native executable, offline CLI and header checks
make sanitize         # ASan + UBSan, separate build/sanitize executables
make clean            # remove owned bot/test executables, not the submodule
```

Use `CC=clang` or `CC=gcc`. Project C compiles as C99 with warnings as errors;
Concord headers are system includes so upstream prototype warnings do not hide
new warnings in bot code. Headers must compile in isolation and tolerate repeat
inclusion. Tests deliberately reject `NDEBUG` so assert-based checks cannot be
silently compiled away. Python 3 is needed for `make check`, not the bot itself.

The event suite links fake `discord_get_data` and `discord_create_message`
functions, not a live client. It captures reply content and asserts explicit
mention suppression. No real token, gateway, user or guild is required. Expected
stderr messages for a simulated queue failure and ready events are not failures.
The property suite uses a fixed seed for 10,000 reproducible boundary cases.

ASan/UBSan builds live separately because Make does not track changes to flags.
After changing compiler/regular CFLAGS, run `make clean` before rebuilding.
The pinned Concord static archive is not sanitizer-instrumented by our target.
For a fresh dependency build, run `make -C concord purge` before `make`.
In particular, do this when copied objects are ELF/x86-64 but the host is macOS
arm64: Make's timestamps do not detect architecture changes.

Live Discord behavior, authentication, platform intent approval and container
runtime behavior require separate manual verification. Passing offline tests
must never be presented as proof of successful live Discord delivery.
