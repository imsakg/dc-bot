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
    /* more tests */
    puts("core: all assertions passed");
    return 0;
}
