#include "bot_core.h"
#include <stdio.h>
#include <string.h>

bool bot_parse(const char *message, const char *prefix, struct bot_input *out)
{
    size_t n = 0, prefix_len;
    if (!message || !prefix || !*prefix || !out) return false;
    prefix_len = strlen(prefix);
    if (strncmp(message, prefix, prefix_len) != 0) return false;
    message += prefix_len;
    while (message[n] && message[n] != ' ' && message[n] != '\t') {
        if (n + 1 >= sizeof out->name) return false;
        out->name[n] = message[n];
        ++n;
    }
    if (!n) return false;
    out->name[n] = '\0';
    message += n;
    while (*message == ' ' || *message == '\t') ++message;
    out->args = message;
    return true;
}

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
