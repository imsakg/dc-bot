#ifndef DC_BOT_CONFIG_H
#define DC_BOT_CONFIG_H
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
struct bot_config {
    char prefix[9];
    unsigned cooldown_seconds;
    uint64_t welcome_guild, welcome_channel;
};
int bot_config_read(struct bot_config *config, FILE *stream, char *error, size_t capacity);
int bot_config_load(struct bot_config *config, const char *path, char *error, size_t capacity);
void bot_config_defaults(struct bot_config *config);
#endif
