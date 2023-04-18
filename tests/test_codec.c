#ifdef NDEBUG
#error "Tests require assertions; do not define NDEBUG"
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "discord.h"
int main(void)
{
    struct strings parse = {0};
    struct discord_allowed_mention mentions = {.parse = &parse, .replied_user = false};
    struct discord_create_message message = {.content = "@everyone <@42> <@&99> %s", .allowed_mentions = &mentions};
    char json[4096];
    size_t length = discord_create_message_to_json(json, sizeof json, &message);
    assert(length > 0 && length < sizeof json);
    assert(strstr(json, "\"allowed_mentions\":{") && strstr(json, "\"parse\":[]"));
    assert(!strstr(json, "\"replied_user\":true"));
    assert(strstr(json, "@everyone <@42> <@&99> %s"));
    puts("codec: explicit empty mention parsing survives Concord JSON encoding");
    return 0;
}
