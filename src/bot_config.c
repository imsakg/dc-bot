#include "bot_config.h"
#include <string.h>
void bot_config_defaults(struct bot_config *config)
{
    if (!config) return;
    memset(config, 0, sizeof *config);
    strcpy(config->prefix, "!");
    config->cooldown_seconds = 2;
}
