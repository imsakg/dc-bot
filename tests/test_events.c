#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "discord.h"
#include "bot.h"
#include "bot_core.h"

static struct bot_state state;
static unsigned sent;
static char last[BOT_REPLY_MAX + 1];
static CCORDcode send_result = CCORD_OK;
void *discord_get_data(struct discord *client) { (void)client; return &state; }
CCORDcode discord_create_message(struct discord *client, u64snowflake channel,
    struct discord_create_message *params, struct discord_ret_message *ret)
{
    (void)client; (void)ret;
    assert(channel != 0 && params && params->content);
    assert(params->allowed_mentions && params->allowed_mentions->parse);
    assert(params->allowed_mentions->parse->size == 0);
    assert(!params->allowed_mentions->replied_user);
    snprintf(last, sizeof last, "%s", params->content);
    ++sent;
    return send_result;
}
int main(void)
{
    struct discord *client = (struct discord *)&state;
    struct discord_user user = {.id = 42, .username = "tester", .discriminator = "0001"};
    struct discord_message event = {.channel_id = 9, .content = "!ping", .author = &user};
    bot_config_defaults(&state.config);
    assert(pthread_mutex_init(&state.mutex, NULL) == 0);
    on_message(client, NULL);
    event.author = NULL; on_message(client, &event); assert(sent == 0);
    event.author = &user; user.bot = true; on_message(client, &event); assert(sent == 0);
    user.bot = false;
    on_message(client, &event); assert(sent == 1 && strcmp(last, "pong") == 0);
    event.content = "hello"; on_message(client, &event); assert(sent == 1);
    event.content = "!echo hi"; on_message(client, &event); assert(sent == 2 && strcmp(last, "hi") == 0);
    struct discord_ready ready = {.user = &user};
    on_ready(client, NULL); assert(state.ready_count == 0);
    on_ready(client, &ready); on_ready(client, &ready);
    assert(state.ready_count == 2);
    assert(state.messages == 3 && state.replies == 2);
    send_result = CCORD_UNAVAILABLE;
    bot_send_reply(client, 9, "test");
    assert(state.replies == 2);
    send_result = CCORD_OK;
    event.content = "!stats";
    on_message(client, &event);
    assert(strstr(last, "Replies queued:") && strstr(last, "Ready events: 2"));
    event.content = "!userinfo";
    on_message(client, &event);
    assert(strcmp(last, "User: tester | ID: 42") == 0);
    event.content = "!serverinfo";
    on_message(client, &event);
    assert(strcmp(last, "This command is only available in a server.") == 0);
    event.guild_id = 123; on_message(client, &event);
    assert(strcmp(last, "Guild ID: 123 | Channel ID: 9") == 0);
    event.guild_id = 0;
    /* event tests */
    pthread_mutex_destroy(&state.mutex);
    puts("events: all assertions passed");
    return 0;
}
