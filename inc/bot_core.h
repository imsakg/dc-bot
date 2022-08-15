#ifndef DC_BOT_CORE_H
#define DC_BOT_CORE_H
#include <stddef.h>
#define BOT_REPLY_MAX 1900
#define BOT_VERSION "0.2.0"
enum bot_result { BOT_ERROR = -1, BOT_IGNORED = 0, BOT_REPLY = 1 };
int bot_respond(const char *name, const char *args, unsigned random,
                char *out, size_t capacity);
#endif
