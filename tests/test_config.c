#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "bot_config.h"
int main(void)
{
    struct bot_config config;
    bot_config_defaults(&config);
    assert(strcmp(config.prefix, "!") == 0);
    assert(config.cooldown_seconds == 2);
    assert(config.welcome_guild == 0 && config.welcome_channel == 0);
    /* config tests */
    puts("config: all assertions passed");
    return 0;
}
