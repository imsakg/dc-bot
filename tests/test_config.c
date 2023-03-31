#ifdef NDEBUG
#error "Tests require assertions; do not define NDEBUG"
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "bot_config.h"
static int load_bytes(struct bot_config *config, const char *text, size_t length)
{
    char error[80];
    FILE *stream = tmpfile();
    assert(stream);
    assert(fwrite(text, 1, length, stream) == length);
    rewind(stream);
    int result = bot_config_read(config, stream, error, sizeof error);
    fclose(stream);
    return result;
}
static int load_text(struct bot_config *config, const char *text)
{
    return load_bytes(config, text, strlen(text));
}
int main(void)
{
    struct bot_config config;
    bot_config_defaults(&config);
    assert(strcmp(config.prefix, "!") == 0);
    assert(config.cooldown_seconds == 2);
    assert(config.welcome_guild == 0 && config.welcome_channel == 0);
    assert(load_text(&config, "# comment\n prefix = !!\r\n") == 0);
    assert(strcmp(config.prefix, "!!") == 0);
    assert(load_text(&config, "prefix=\n") != 0);
    assert(load_text(&config, "prefix=123456789\n") != 0);
    strcpy(config.prefix, "!");
    assert(load_text(&config, "prefix=?\nunknown=yes\n") != 0);
    assert(strcmp(config.prefix, "!") == 0);
    assert(load_text(&config, "prefix=?\nprefix=!!\n") != 0);
    assert(strcmp(config.prefix, "!") == 0);
    assert(load_text(&config, "welcome_guild=123\nwelcome_channel=456\n") == 0);
    assert(config.welcome_guild == 123 && config.welcome_channel == 456);
    assert(load_text(&config, "welcome_guild=0\nwelcome_channel=0\n") == 0);
    assert(load_text(&config, "welcome_guild=123\n") != 0);
    assert(load_text(&config, "welcome_guild=-1\n") != 0);
    assert(load_text(&config, "cooldown_seconds=0\n") == 0 && config.cooldown_seconds == 0);
    assert(load_text(&config, "cooldown_seconds=3600\n") == 0 && config.cooldown_seconds == 3600);
    assert(load_text(&config, "cooldown_seconds=3601\n") != 0);
    assert(load_text(&config, "cooldown_seconds=-2\n") != 0);
    assert(load_text(&config, "cooldown_seconds=1\ncooldown_seconds=2\n") != 0);
    const char binary[] = "prefix=?\0unknown=yes";
    const char binary_newline[] = "prefix=?\0unknown=yes\n";
    struct bot_config before = config;
    assert(load_bytes(&config, binary, sizeof binary - 1) != 0);
    assert(memcmp(&config, &before, sizeof config) == 0);
    assert(load_bytes(&config, binary_newline, sizeof binary_newline - 1) != 0);
    assert(memcmp(&config, &before, sizeof config) == 0);
    /* config tests */
    puts("config: all assertions passed");
    return 0;
}
