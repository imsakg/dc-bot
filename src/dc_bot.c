#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "discord.h"
#include "log.h"
#include "bot.h"
#include "bot_core.h"
#include "bot_config.h"

int main(int argc, char *argv[])
{
    const char *config_file = "config.json", *settings_file = NULL;
    bool check_only = false, positional = false;
    struct bot_config settings;
    char error[128];
    bot_config_defaults(&settings);
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--help") == 0) { print_usage(); return EXIT_SUCCESS; }
        if (strcmp(argv[i], "--version") == 0) { puts("dc-bot " BOT_VERSION); return EXIT_SUCCESS; }
        if (strcmp(argv[i], "--check-config") == 0) { check_only = true; continue; }
        if (strcmp(argv[i], "--settings") == 0 || strcmp(argv[i], "--config") == 0) {
            bool is_settings = strcmp(argv[i], "--settings") == 0;
            if (++i == argc) { fputs("Missing option value.\n", stderr); return EXIT_FAILURE; }
            if (is_settings) settings_file = argv[i]; else config_file = argv[i];
        } else if (argv[i][0] != '-' && !positional) {
            config_file = argv[i]; positional = true;
        } else { fputs("Unknown option or extra argument.\n", stderr); return EXIT_FAILURE; }
    }
    if (settings_file && bot_config_load(&settings, settings_file, error, sizeof error) != 0) {
        fprintf(stderr, "%s\n", error); return EXIT_FAILURE;
    }
    if (check_only) {
        printf("Settings valid: prefix=%s cooldown_seconds=%u\n", settings.prefix, settings.cooldown_seconds);
        return EXIT_SUCCESS;
    }

    const char *token = getenv("DISCORD_TOKEN");
    if (token && !bot_token_valid(token)) {
        fputs("DISCORD_TOKEN is empty, a placeholder or contains invalid characters.\n", stderr);
        return EXIT_FAILURE;
    }
    if (!token) {
        FILE *probe = fopen(config_file, "r");
        if (!probe) { fputs("Cannot read the Concord config file; set DISCORD_TOKEN or --config.\n", stderr); return EXIT_FAILURE; }
        fclose(probe);
    }
    if (ccord_global_init() != CCORD_OK) {
        fputs("Concord global initialization failed.\n", stderr); return EXIT_FAILURE;
    }
    struct discord *client = token ? discord_init(token) : discord_config_init(config_file);
    if (!client) {
        fputs("Discord client initialization failed.\n", stderr);
        ccord_global_cleanup(); return EXIT_FAILURE;
    }

    struct bot_state state = {.config = settings, .started = time(NULL)};
    if (pthread_mutex_init(&state.mutex, NULL) != 0) {
        discord_cleanup(client); ccord_global_cleanup(); return EXIT_FAILURE;
    }
    srand((unsigned)state.started);
    discord_set_data(client, &state);
    discord_add_intents(client, DISCORD_GATEWAY_GUILDS | DISCORD_GATEWAY_GUILD_MESSAGES |
        DISCORD_GATEWAY_DIRECT_MESSAGES | DISCORD_GATEWAY_MESSAGE_CONTENT);
    discord_set_on_ready(client, &on_ready);
    discord_set_on_message_create(client, &on_message);

    CCORDcode result = discord_run(client);
    discord_cleanup(client);
    pthread_mutex_destroy(&state.mutex);
    ccord_global_cleanup();
    return result == CCORD_OK ? EXIT_SUCCESS : EXIT_FAILURE;
}

void print_usage(void)
{
    puts("Usage: dc_bot [--config config.json] [--settings bot.conf] [--check-config]\n"
         "       dc_bot --help | --version\n"
         "DISCORD_TOKEN overrides the Concord JSON credential file.");
}


