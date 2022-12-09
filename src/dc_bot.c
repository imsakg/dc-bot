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
    ccord_global_init();
    struct discord *client = token ? discord_init(token) : discord_config_init(config_file);

    discord_set_on_ready(client, &on_ready);
    discord_set_on_command(client, "ping", &on_ping);
    discord_set_on_command(client, "pong", &on_pong);

    print_usage();
    fgetc(stdin); // wait for input

    discord_run(client);

    discord_cleanup(client);
    ccord_global_cleanup();
}

void print_usage(void)
{
    puts("Usage: dc_bot [--config config.json] [--settings bot.conf] [--check-config]\n"
         "       dc_bot --help | --version\n"
         "DISCORD_TOKEN overrides the Concord JSON credential file.");
}

void on_ready(struct discord *client, const struct discord_ready *event)
{
    log_info("PingPong-Bot succesfully connected to Discord as %s#%s!",
             event->user->username, event->user->discriminator);
}

void on_ping(struct discord *client, const struct discord_message *event)
{
    if (event->author->bot)
        return;

    char reply[32];
    bot_respond("ping", "", 0, reply, sizeof reply);
    struct discord_create_message params = {.content = reply};
    discord_create_message(client, event->channel_id, &params, NULL);
}

void on_pong(struct discord *client, const struct discord_message *event)
{
    if (event->author->bot)
        return;

    char reply[32];
    bot_respond("pong", "", 0, reply, sizeof reply);
    struct discord_create_message params = {.content = reply};
    discord_create_message(client, event->channel_id, &params, NULL);
}
