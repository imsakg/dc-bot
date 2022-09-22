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
    char out[32];
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
    /* more tests */
    puts("core: all assertions passed");
    return 0;
}
