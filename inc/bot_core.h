#ifndef DC_BOT_CORE_H
#define DC_BOT_CORE_H
#include <stddef.h>
#include <stdbool.h>
struct bot_input { char name[24]; const char *args; };
bool bot_parse(const char *message, const char *prefix, struct bot_input *out);
#define BOT_REPLY_MAX 1900
#define BOT_VERSION "0.2.0"
#define BOT_COMMANDS "ping pong help about echo"
bool bot_known_command(const char *name);
enum bot_result { BOT_ERROR = -1, BOT_IGNORED = 0, BOT_REPLY = 1 };
int bot_respond(const char *name, const char *args, unsigned random,
                char *out, size_t capacity);
#endif
