#ifdef NDEBUG
#error "Tests require assertions; do not define NDEBUG"
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "bot_core.h"

static uint32_t seed = UINT32_C(0x5eed);
static uint32_t next(void) { seed = seed * UINT32_C(1664525) + UINT32_C(1013904223); return seed; }
int main(void)
{
    static const char *const names[] = {"ping", "pong", "help", "about", "echo", "upper", "lower", "choose", "roll", "coin", "eightball", "snowflake", "unknown"};
    for (unsigned run = 0; run < 10000; ++run) {
        char message[257];
        size_t length = next() % (sizeof message - 1);
        for (size_t i = 0; i < length; ++i) message[i] = (char)(1 + next() % 255);
        message[length] = '\0';
        struct bot_input parsed;
        if (bot_parse(message, "!", &parsed)) {
            assert(message[0] == '!');
            assert(strlen(parsed.name) > 0 && strlen(parsed.name) < sizeof parsed.name);
            for (size_t i = 0; parsed.name[i]; ++i) assert(parsed.name[i] >= 'a' && parsed.name[i] <= 'z');
            assert(parsed.args >= message && parsed.args <= message + length);
        }
        struct guarded { unsigned char left[16]; char text[32]; unsigned char right[16]; } buffer;
        size_t capacity = 1 + next() % sizeof buffer.text;
        memset(&buffer, 0xa5, sizeof buffer);
        int result = bot_respond(names[run % (sizeof names / sizeof *names)], message, next(), buffer.text, capacity);
        for (size_t i = 0; i < 16; ++i) assert(buffer.left[i] == 0xa5 && buffer.right[i] == 0xa5);
        if (result == BOT_REPLY) assert(strlen(buffer.text) < capacity);
        else assert(buffer.text[0] == '\0');
        for (size_t i = capacity; i < sizeof buffer.text; ++i) assert((unsigned char)buffer.text[i] == 0xa5);
        uint64_t id = ((uint64_t)next() << 32) | next(), parsed_id = 0;
        char decimal[32];
        snprintf(decimal, sizeof decimal, "%llu", (unsigned long long)id);
        assert(bot_id(decimal, &parsed_id) == (id != 0));
        if (id) assert(parsed_id == id);
    }
    puts("properties: 10000 deterministic cases passed");
    return 0;
}
