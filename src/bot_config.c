#include "bot_config.h"
#include <string.h>
#include <ctype.h>
static int config_error(char *error, size_t capacity, unsigned line)
{
    if (error && capacity) snprintf(error, capacity, "Invalid bot setting at line %u", line);
    return -1;
}

static char *trim(char *text)
{
    while (isspace((unsigned char)*text)) ++text;
    size_t length = strlen(text);
    while (length && isspace((unsigned char)text[length - 1])) text[--length] = '\0';
    return text;
}

int bot_config_read(struct bot_config *config, FILE *stream, char *error, size_t capacity)
{
    char line[256]; unsigned number = 0;
    if (!config || !stream) return config_error(error, capacity, 0);
    struct bot_config next = *config;
    unsigned seen = 0;
    if (error && capacity) *error = '\0';
    while (fgets(line, sizeof line, stream)) {
        ++number;
        if (!strchr(line, '\n') && !feof(stream)) return config_error(error, capacity, number);
        char *key = trim(line);
        if (!*key || *key == '#') continue;
        char *equals = strchr(key, '=');
        if (!equals) return config_error(error, capacity, number);
        *equals = '\0'; key = trim(key);
        char *value = trim(equals + 1);
        if (strcmp(key, "prefix") == 0) {
            if (seen & 1U) return config_error(error, capacity, number);
            seen |= 1U;
            size_t n = strlen(value);
            if (!n || n >= sizeof next.prefix) return config_error(error, capacity, number);
            for (size_t i = 0; i < n; ++i)
                if ((unsigned char)value[i] <= 32 || (unsigned char)value[i] > 126)
                    return config_error(error, capacity, number);
            memcpy(next.prefix, value, n + 1);
        } else return config_error(error, capacity, number);
    }
    if (ferror(stream)) return config_error(error, capacity, number);
    *config = next;
    return 0;
}

int bot_config_load(struct bot_config *config, const char *path, char *error, size_t capacity)
{
    FILE *stream = path ? fopen(path, "r") : NULL;
    if (!stream) return config_error(error, capacity, 0);
    int result = bot_config_read(config, stream, error, capacity);
    fclose(stream);
    return result;
}

void bot_config_defaults(struct bot_config *config)
{
    if (!config) return;
    memset(config, 0, sizeof *config);
    strcpy(config->prefix, "!");
    config->cooldown_seconds = 2;
}
