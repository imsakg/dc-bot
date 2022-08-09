SUB = concord
CC = cc
CPPFLAGS += -Iinc -I$(SUB)/include -I$(SUB)/core -I$(SUB)/gencodecs
CFLAGS ?= -O2 -g
CFLAGS += -std=c99 -Wall -Wextra -Wpedantic -pthread
LDFLAGS += -L$(SUB)/lib
LDLIBS += -ldiscord -lcurl -pthread
BOT = build/dc_bot
SOURCES = src/dc_bot.c

all: $(BOT)

$(SUB)/lib/libdiscord.a:
	$(MAKE) -C $(SUB) CC="$(CC)"

$(BOT): $(SOURCES) inc/bot.h $(SUB)/lib/libdiscord.a | build
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) $(LDFLAGS) $(LDLIBS) -o $@

build:
	mkdir -p $@

run: $(BOT)
	./$(BOT) $(CONFIG)

clean:
	rm -f $(BOT)

.PHONY: all run clean
