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
    /* more tests */
    puts("core: all assertions passed");
    return 0;
}
