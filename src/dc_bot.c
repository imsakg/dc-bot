#include <stdio.h>
#include <stdlib.h>

#include "discord.h"
#include "log.h"
#include "bot.h"
#include "bot_core.h"

int main(int argc, char *argv[])
{
    const char *config_file;
    if (argc > 1)
        config_file = argv[1];
    else
        config_file = "config.json";

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
    printf("\n\nThis bot demonstrates a simple ping-pong response.\n"
           "1. Type 'pong' in chat\n"
           "2. Type 'ping' in chat\n"
           "\nTYPE ANY KEY TO START BOT\n");
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
