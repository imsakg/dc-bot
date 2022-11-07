#ifndef DC_BOT_CONFIG_H
#define DC_BOT_CONFIG_H
#include <stdint.h>
struct bot_config {
    char prefix[9];
    unsigned cooldown_seconds;
    uint64_t welcome_guild, welcome_channel;
};
void bot_config_defaults(struct bot_config *config);
#endif
