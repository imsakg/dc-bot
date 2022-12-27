#include "discord.h"
#include "bot.h"
#include "bot_core.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void bot_send_reply(struct discord *client, uint64_t channel, char *text)
{
    if (!client || !channel || !text || !*text || strlen(text) > BOT_REPLY_MAX) return;
    struct strings parse = {0};
    struct discord_allowed_mention mentions = {.parse = &parse, .replied_user = false};
    struct discord_create_message params = {.content = text, .allowed_mentions = &mentions};
    if (discord_create_message(client, channel, &params, NULL) != CCORD_OK) {
        fputs("Unable to queue a Discord reply.\n", stderr);
    } else {
        struct bot_state *state = discord_get_data(client);
        if (state) ++state->replies;
    }
}

void on_ready(struct discord *client, const struct discord_ready *event)
{
    if (!client || !event || !event->user) return;
    struct bot_state *state = discord_get_data(client);
    if (state) ++state->ready_count;
    fprintf(stderr, "dc-bot connected as %s\n", event->user->username ? event->user->username : "unknown");
}

void on_message(struct discord *client, const struct discord_message *event)
{
    if (!client || !event || !event->author || event->author->bot || !event->content) return;
    struct bot_state *state = discord_get_data(client);
    struct bot_input input;
    char reply[BOT_REPLY_MAX + 1];
    if (!state) return;
    ++state->messages;
    if (!bot_parse(event->content, state->config.prefix, &input)) return;
    if (bot_respond(input.name, input.args, (unsigned)rand(), reply, sizeof reply) == BOT_REPLY)
        bot_send_reply(client, event->channel_id, reply);
}
