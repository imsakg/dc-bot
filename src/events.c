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

void on_member_remove(struct discord *client, const struct discord_guild_member_remove *event)
{
    if (!client || !event || !event->user || event->user->bot) return;
    struct bot_state *state = discord_get_data(client);
    char reply[BOT_REPLY_MAX + 1];
    if (!state || !state->config.welcome_channel || event->guild_id != state->config.welcome_guild) return;
    if (bot_format(reply, sizeof reply, "Goodbye, %s.", event->user->username ? event->user->username : "member") == BOT_REPLY)
        bot_send_reply(client, state->config.welcome_channel, reply);
}

void on_member_add(struct discord *client, const struct discord_guild_member *event)
{
    if (!client || !event || !event->user || event->user->bot) return;
    struct bot_state *state = discord_get_data(client);
    char reply[BOT_REPLY_MAX + 1];
    if (!state || !state->config.welcome_channel || event->guild_id != state->config.welcome_guild) return;
    if (bot_format(reply, sizeof reply, "Welcome, %s!", event->user->username ? event->user->username : "new member") == BOT_REPLY)
        bot_send_reply(client, state->config.welcome_channel, reply);
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
    if (strcmp(input.name, "stats") == 0) {
        if (bot_format(reply, sizeof reply, "Messages: %lu | Replies queued: %lu | Ready events: %lu",
            state->messages, state->replies, state->ready_count) == BOT_REPLY)
            bot_send_reply(client, event->channel_id, reply);
        return;
    }
    if (strcmp(input.name, "userinfo") == 0) {
        if (bot_format(reply, sizeof reply, "User: %s | ID: %llu",
            event->author->username ? event->author->username : "unknown",
            (unsigned long long)event->author->id) == BOT_REPLY)
            bot_send_reply(client, event->channel_id, reply);
        return;
    }
    if (strcmp(input.name, "serverinfo") == 0) {
        int result = event->guild_id ?
            bot_format(reply, sizeof reply, "Guild ID: %llu | Channel ID: %llu",
                (unsigned long long)event->guild_id, (unsigned long long)event->channel_id) :
            bot_format(reply, sizeof reply, "This command is only available in a server.");
        if (result == BOT_REPLY) bot_send_reply(client, event->channel_id, reply);
        return;
    }
    if (strcmp(input.name, "avatar") == 0) {
        if (bot_avatar(event->author->id, event->author->avatar, event->author->discriminator,
            reply, sizeof reply) == BOT_REPLY) bot_send_reply(client, event->channel_id, reply);
        return;
    }
    /* context handlers */
    if (strcmp(input.name, "uptime") == 0) {
        time_t now = time(NULL);
        uint64_t elapsed = now > state->started ? (uint64_t)(now - state->started) : 0;
        if (bot_uptime(elapsed, reply, sizeof reply) == BOT_REPLY) bot_send_reply(client, event->channel_id, reply);
        return;
    }
    if (bot_respond(input.name, input.args, (unsigned)rand(), reply, sizeof reply) == BOT_REPLY)
        bot_send_reply(client, event->channel_id, reply);
}
