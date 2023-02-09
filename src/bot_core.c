#include "bot_core.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

bool bot_allow(struct bot_limiter *limiter, uint64_t user, uint64_t channel, uint64_t now, unsigned delay)
{
    size_t candidate = 0;
    if (!limiter || !user || !channel) return false;
    if (!delay) return true;
    for (size_t i = 0; i < BOT_RATE_SLOTS; ++i) {
        struct bot_rate_slot *slot = &limiter->slots[i];
        if (slot->used && slot->user == user && slot->channel == channel) {
            if (now >= slot->last && now - slot->last < delay) return false;
            slot->last = now;
            return true;
        }
        if (!slot->used || (limiter->slots[candidate].used && slot->last < limiter->slots[candidate].last)) candidate = i;
    }
    limiter->slots[candidate] = (struct bot_rate_slot){.user = user, .channel = channel, .last = now, .used = true};
    return true;
}

int bot_avatar(uint64_t id, const char *avatar, const char *discriminator, char *out, size_t capacity)
{
    if (!id) return BOT_ERROR;
    if (!avatar || !*avatar) {
        unsigned long legacy = 0;
        bool old = bot_number(discriminator, 1, 9999, &legacy);
        unsigned index = old ? (unsigned)(legacy % 5) : (unsigned)((id >> 22) % 6);
        return bot_format(out, capacity, "https://cdn.discordapp.com/embed/avatars/%u.png", index);
    }
    bool animated = strncmp(avatar, "a_", 2) == 0;
    const char *hash = avatar + (animated ? 2 : 0);
    size_t n = strlen(hash);
    if (!n || n > 64) return BOT_ERROR;
    for (size_t i = 0; i < n; ++i) {
        char c = hash[i];
        if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) return BOT_ERROR;
    }
    return bot_format(out, capacity, "https://cdn.discordapp.com/avatars/%llu/%s.%s?size=256",
        (unsigned long long)id, avatar, animated ? "gif" : "png");
}

int bot_uptime(uint64_t seconds, char *out, size_t capacity)
{
    return bot_format(out, capacity, "Uptime: %llud %lluh %llum %llus",
        (unsigned long long)(seconds / 86400), (unsigned long long)(seconds / 3600 % 24),
        (unsigned long long)(seconds / 60 % 60), (unsigned long long)(seconds % 60));
}

bool bot_token_valid(const char *token)
{
    size_t n;
    if (!token || !(n = strlen(token)) || n > 4094 || strcmp(token, "YOUR-BOT-TOKEN") == 0) return false;
    for (size_t i = 0; i < n; ++i) {
        unsigned char c = (unsigned char)token[i];
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
              (c >= '0' && c <= '9') || c == '.' || c == '_' || c == '-')) return false;
    }
    return true;
}

bool bot_id(const char *text, uint64_t *out)
{
    uint64_t value = 0;
    if (!text || !*text || !out) return false;
    for (const unsigned char *p = (const unsigned char *)text; *p; ++p) {
        unsigned digit = (unsigned)(*p - '0');
        if (*p < '0' || *p > '9' || value > (UINT64_MAX - digit) / 10) return false;
        value = value * 10 + digit;
    }
    if (!value) return false;
    *out = value;
    return true;
}

bool bot_number(const char *text, unsigned long min, unsigned long max, unsigned long *out)
{
    unsigned long value = 0;
    if (!text || !*text || !out || min > max) return false;
    for (const unsigned char *p = (const unsigned char *)text; *p; ++p) {
        unsigned digit = (unsigned)(*p - '0');
        if (*p < '0' || *p > '9' || digit > max || value > (max - digit) / 10) return false;
        value = value * 10 + digit;
    }
    if (value < min) return false;
    *out = value;
    return true;
}

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
        unsigned long sides = 6;
        if (args && *args && !bot_number(args, 2, 1000000, &sides))
            return bot_format(out, capacity, "Usage: roll [sides: 2-1000000]");
        return bot_format(out, capacity, "Rolled %lu (1-%lu)", 1UL + random % sides, sides);
    }
    if (strcmp(name, "coin") == 0) text = random % 2 ? "tails" : "heads";
    if (strcmp(name, "eightball") == 0) {
        static const char *const answers[] = {"Yes.", "No.", "Maybe.", "Ask again later.", "It looks promising.", "Unlikely."};
        text = !args || !*args ? "Usage: eightball <question>" : answers[random % 6];
    }
    if (strcmp(name, "snowflake") == 0) {
        uint64_t id;
        if (!bot_id(args, &id)) return bot_format(out, capacity, "Usage: snowflake <positive Discord ID>");
        return bot_format(out, capacity, "<t:%llu:f>",
            (unsigned long long)(((id >> 22) + UINT64_C(1420070400000)) / 1000));
    }
    /* command handlers */
    if (strcmp(name, "ping") == 0) text = "pong";
    if (strcmp(name, "pong") == 0) text = "ping";
    if (!text) return BOT_IGNORED;
    return bot_format(out, capacity, "%s", text);
}
