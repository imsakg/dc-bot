#ifndef DC_BOT_H
#define DC_BOT_H
#include <pthread.h>
#include <time.h>
#include "bot_config.h"
struct discord;
struct discord_ready;
struct discord_message;
struct discord_guild;
struct discord_guild_member;
struct discord_guild_member_remove;
struct bot_state {
    struct bot_config config;
    time_t started;
    unsigned long messages, replies, ready_count, guild_events;
    pthread_mutex_t mutex;
};
void print_usage(void);
void on_ready(struct discord *client, const struct discord_ready *event);
void on_member_add(struct discord *client, const struct discord_guild_member *event);
void on_message(struct discord *client, const struct discord_message *event);
void bot_send_reply(struct discord *client, uint64_t channel, char *text);
#endif
