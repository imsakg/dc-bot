SUB = concord
CC = cc
CPPFLAGS += -Iinc -I$(SUB)/include -I$(SUB)/core -I$(SUB)/gencodecs
CFLAGS ?= -O2 -g
CFLAGS += -std=c99 -Wall -Wextra -Wpedantic -pthread
LDFLAGS += -L$(SUB)/lib
LDLIBS += -ldiscord -lcurl -pthread
BOT = build/dc_bot
SOURCES = src/dc_bot.c src/bot_core.c src/bot_config.c src/events.c
CORE = src/bot_core.c
HEADERS = $(wildcard inc/*.h)
SANFLAGS = -O1 -g -std=c99 -Wall -Wextra -Wpedantic -pthread -fsanitize=address,undefined -fno-omit-frame-pointer

all: $(BOT)

$(SUB)/lib/libdiscord.a:
	$(MAKE) -C $(SUB) CC="$(CC)"

$(BOT): $(SOURCES) $(HEADERS) $(SUB)/lib/libdiscord.a | build
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) $(LDFLAGS) $(LDLIBS) -o $@

build:
	mkdir -p $@

run: $(BOT)
	./$(BOT) $(CONFIG)

clean:
	rm -f $(BOT) build/test_core build/test_config build/test_events build/test_properties build/test_codec
	rm -rf build/sanitize

build/test_core: tests/test_core.c $(CORE) inc/bot_core.h | build
	$(CC) -Iinc $(CFLAGS) tests/test_core.c $(CORE) -o $@

build/test_config: tests/test_config.c src/bot_config.c $(CORE) inc/bot_config.h inc/bot_core.h | build
	$(CC) -Iinc $(CFLAGS) tests/test_config.c src/bot_config.c $(CORE) -o $@

build/test_events: tests/test_events.c src/events.c src/bot_config.c $(CORE) $(HEADERS) | build
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/test_events.c src/events.c src/bot_config.c $(CORE) -o $@

test: build/test_core build/test_config build/test_events
	./build/test_core
	./build/test_config
	./build/test_events

check: all test
	python3 tests/test_cli.py

build/sanitize:
	mkdir -p $@

build/sanitize/test_core: tests/test_core.c $(CORE) $(HEADERS) | build/sanitize
	$(CC) -Iinc $(SANFLAGS) tests/test_core.c $(CORE) -o $@

build/sanitize/test_config: tests/test_config.c src/bot_config.c $(CORE) $(HEADERS) | build/sanitize
	$(CC) -Iinc $(SANFLAGS) tests/test_config.c src/bot_config.c $(CORE) -o $@

build/sanitize/test_events: tests/test_events.c src/events.c src/bot_config.c $(CORE) $(HEADERS) | build/sanitize
	$(CC) $(CPPFLAGS) $(SANFLAGS) tests/test_events.c src/events.c src/bot_config.c $(CORE) -o $@

sanitize: build/sanitize/test_core build/sanitize/test_config build/sanitize/test_events
	./build/sanitize/test_core
	./build/sanitize/test_config
	./build/sanitize/test_events

.PHONY: all run clean test check sanitize
