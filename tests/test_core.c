#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "bot_core.h"

static void expect(const char *name, const char *args, unsigned random,
                   const char *wanted)
{
    char out[BOT_REPLY_MAX + 1];
    assert(bot_respond(name, args, random, out, sizeof out) == BOT_REPLY);
    assert(strcmp(out, wanted) == 0);
}

int main(void)
{
    expect("ping", "", 0, "pong");
    expect("pong", "", 0, "ping");
    char out[BOT_REPLY_MAX + 1];
    assert(bot_respond("missing", "", 0, out, sizeof out) == BOT_IGNORED);
    struct bot_input parsed;
    assert(bot_parse("!ping hello", "!", &parsed));
    assert(strcmp(parsed.name, "ping") == 0);
    assert(strcmp(parsed.args, "hello") == 0);
    assert(!bot_parse("ping", "!", &parsed));
    assert(!bot_parse("!", "!", &parsed));
    assert(!bot_parse("!pingpong", "!", &parsed) ||
           bot_respond(parsed.name, parsed.args, 0, out, sizeof out) == BOT_IGNORED);
    assert(!bot_parse("!this_command_name_is_way_too_long", "!", &parsed));
    assert(!bot_parse("!ping\n", "!", &parsed));
    assert(!bot_parse("!PING", "!", &parsed));
    assert(bot_parse("!!echo\t hi", "!!", &parsed));
    assert(strcmp(parsed.args, "hi") == 0);
    assert(bot_respond("help", "", 0, out, sizeof out) == BOT_REPLY);
    assert(strstr(out, "ping") && strstr(out, "help"));
    assert(bot_known_command("ping"));
    assert(!bot_known_command("pin"));
    expect("about", "", 0, "dc-bot " BOT_VERSION " | C + Concord");
    expect("echo", "hello C", 0, "hello C");
    expect("echo", "", 0, "Usage: echo <text>");
    char tiny[4] = "xxx";
    assert(bot_respond("ping", "", 0, tiny, sizeof tiny) == BOT_ERROR);
    assert(tiny[0] == '\0');
    char large[BOT_REPLY_MAX + 2];
    memset(large, 'a', sizeof large - 1); large[sizeof large - 1] = '\0';
    assert(bot_respond("echo", large, 0, out, sizeof out) == BOT_ERROR);
    assert(out[0] == '\0');
    expect("echo", "%s%n", 0, "%s%n");
    expect("upper", "aZ 19 %", 0, "AZ 19 %");
    expect("upper", "café", 0, "CAFé");
    expect("upper", "", 0, "Usage: upper <text>");
    expect("lower", "C Is FUN!", 0, "c is fun!");
    expect("lower", "", 0, "Usage: lower <text>");
    expect("choose", "tea | coffee | water", 1, "coffee");
    expect("choose", "tea | coffee", 2, "tea");
    expect("choose", "tea || water", 0,
           "Usage: choose <a> | <b> [| ...] (2-16 choices)");
    expect("roll", "", 5, "Rolled 6 (1-6)");
    expect("roll", "20", 19, "Rolled 20 (1-20)");
    expect("roll", "1", 0, "Usage: roll [sides: 2-1000000]");
    expect("roll", "oops", 0, "Usage: roll [sides: 2-1000000]");
    expect("coin", "", 0, "heads");
    expect("coin", "", 3, "tails");
    expect("eightball", "Will it build?", 0, "Yes.");
    expect("eightball", "", 0, "Usage: eightball <question>");
    expect("eightball", "Will it build?", 7, "No.");
    unsigned long number = 123;
    assert(bot_number("100", 2, 100, &number) && number == 100);
    assert(!bot_number("+6", 2, 100, &number));
    assert(!bot_number(" 6", 2, 100, &number));
    assert(!bot_number("999999999999999999999999", 0, 100, &number));
    expect("roll", "+6", 0, "Usage: roll [sides: 2-1000000]");
    /* more tests */
    puts("core: all assertions passed");
    return 0;
}
