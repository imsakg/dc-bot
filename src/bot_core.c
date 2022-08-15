#include "bot_core.h"
#include <stdio.h>
#include <string.h>

int bot_respond(const char *name, const char *args, unsigned random,
                char *out, size_t capacity)
{
    const char *text = NULL;
    (void)args;
    (void)random;
    if (!name || !out || capacity == 0) return BOT_ERROR;
    out[0] = '\0';
    /* command handlers */
    if (strcmp(name, "ping") == 0) text = "pong";
    if (strcmp(name, "pong") == 0) text = "ping";
    if (!text) return BOT_IGNORED;
    snprintf(out, capacity, "%s", text);
    return BOT_REPLY;
}
