#include "bot_core.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

int bot_format(char *out, size_t capacity, const char *format, ...)
{
    int n;
    va_list args;
    if (!out || !capacity || !format) return BOT_ERROR;
    va_start(args, format);
    n = vsnprintf(out, capacity, format, args);
    va_end(args);
    if (n < 0 || (size_t)n >= capacity || n > BOT_REPLY_MAX) {
        out[0] = '\0';
        return BOT_ERROR;
    }
    return BOT_REPLY;
}

bool bot_parse(const char *message, const char *prefix, struct bot_input *out)
{
    size_t n = 0, prefix_len;
    if (!message || !prefix || !*prefix || !out) return false;
    prefix_len = strlen(prefix);
    if (strncmp(message, prefix, prefix_len) != 0) return false;
    message += prefix_len;
    while (message[n] && message[n] != ' ' && message[n] != '\t') {
        if (n + 1 >= sizeof out->name) return false;
        if (message[n] < 'a' || message[n] > 'z') return false;
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

bool bot_known_command(const char *name)
{
    const char *p = BOT_COMMANDS;
    size_t n;
    if (!name || !*name) return false;
    n = strlen(name);
    while (*p) {
        const char *end = strchr(p, ' ');
        size_t length = end ? (size_t)(end - p) : strlen(p);
        if (length == n && strncmp(name, p, n) == 0) return true;
        if (!end) break;
        p = end + 1;
    }
    return false;
}

int bot_respond(const char *name, const char *args, unsigned random,
                char *out, size_t capacity)
{
    const char *text = NULL;
    (void)args;
    (void)random;
    if (!name || !out || capacity == 0) return BOT_ERROR;
    out[0] = '\0';
    if (strcmp(name, "help") == 0) text = BOT_COMMANDS;
    if (strcmp(name, "about") == 0) text = "dc-bot " BOT_VERSION " | C + Concord";
    if (strcmp(name, "echo") == 0) text = args && *args ? args : "Usage: echo <text>";
    if (strcmp(name, "upper") == 0) {
        if (!args || !*args) return bot_format(out, capacity, "Usage: upper <text>");
        if (bot_format(out, capacity, "%s", args) != BOT_REPLY) return BOT_ERROR;
        for (size_t i = 0; out[i]; ++i)
            if (out[i] >= 'a' && out[i] <= 'z') out[i] -= 'a' - 'A';
        return BOT_REPLY;
    }
    if (strcmp(name, "lower") == 0) {
        if (!args || !*args) return bot_format(out, capacity, "Usage: lower <text>");
        if (bot_format(out, capacity, "%s", args) != BOT_REPLY) return BOT_ERROR;
        for (size_t i = 0; out[i]; ++i)
            if (out[i] >= 'A' && out[i] <= 'Z') out[i] += 'a' - 'A';
        return BOT_REPLY;
    }
    if (strcmp(name, "choose") == 0) {
        const char *starts[16]; size_t lengths[16], count = 0;
        const char *p = args ? args : "";
        do {
            const char *end = strchr(p, '|');
            const char *last = end ? end : p + strlen(p);
            while (p < last && (*p == ' ' || *p == '\t')) ++p;
            while (last > p && (last[-1] == ' ' || last[-1] == '\t')) --last;
            if (p == last || count == 16)
                return bot_format(out, capacity, "Usage: choose <a> | <b> [| ...] (2-16 choices)");
            starts[count] = p; lengths[count++] = (size_t)(last - p);
            if (!end) break;
            p = end + 1;
        } while (1);
        if (count < 2) return bot_format(out, capacity, "Usage: choose <a> | <b> [| ...] (2-16 choices)");
        size_t pick = random % count;
        if (lengths[pick] > BOT_REPLY_MAX) return BOT_ERROR;
        return bot_format(out, capacity, "%.*s", (int)lengths[pick], starts[pick]);
    }
    if (strcmp(name, "roll") == 0) {
        char *end = NULL;
        unsigned long sides = args && *args ? strtoul(args, &end, 10) : 6;
        if (sides < 2 || sides > 1000000 || (end && *end))
            return bot_format(out, capacity, "Usage: roll [sides: 2-1000000]");
        return bot_format(out, capacity, "Rolled %lu (1-%lu)", 1UL + random % sides, sides);
    }
    /* command handlers */
    if (strcmp(name, "ping") == 0) text = "pong";
    if (strcmp(name, "pong") == 0) text = "ping";
    if (!text) return BOT_IGNORED;
    return bot_format(out, capacity, "%s", text);
}
