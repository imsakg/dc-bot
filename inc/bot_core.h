#ifndef DC_BOT_CORE_H
#define DC_BOT_CORE_H
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#define BOT_RATE_SLOTS 64
struct bot_rate_slot { uint64_t user, channel, last; bool used; };
struct bot_limiter { struct bot_rate_slot slots[BOT_RATE_SLOTS]; };
bool bot_allow(struct bot_limiter *limiter, uint64_t user, uint64_t channel, uint64_t now, unsigned delay);
struct bot_input { char name[24]; const char *args; };
bool bot_parse(const char *message, const char *prefix, struct bot_input *out);
#define BOT_REPLY_MAX 1900
#define BOT_VERSION "0.2.0"
#define BOT_COMMANDS "ping pong help about echo upper lower choose roll coin eightball snowflake uptime stats userinfo serverinfo avatar"
bool bot_known_command(const char *name);
enum bot_result { BOT_ERROR = -1, BOT_IGNORED = 0, BOT_REPLY = 1 };
int bot_avatar(uint64_t id, const char *avatar, const char *discriminator, char *out, size_t capacity);
int bot_uptime(uint64_t seconds, char *out, size_t capacity);
bool bot_token_valid(const char *token);
bool bot_id(const char *text, uint64_t *out);
bool bot_number(const char *text, unsigned long min, unsigned long max, unsigned long *out);
int bot_format(char *out, size_t capacity, const char *format, ...);
int bot_respond(const char *name, const char *args, unsigned random,
                char *out, size_t capacity);
#endif
