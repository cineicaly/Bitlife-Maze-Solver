#include "parse.h"
#include "cJSON.h"
#include <stdio.h>
#include <stdlib.h>

int get_safe_int(cJSON *parent, const char *key, int *out_val)
{
    cJSON *item = cJSON_GetObjectItemCaseSensitive(parent, key);
    if (cJSON_IsNumber(item)) {
        *out_val = item->valueint;
        return SUCCESS;
    }
    printf("Error: '%s' is missing or not a valid number.\n", key);
    return INT_READ_ERR;
}

int parse_json(const char *raw_json, maze_struct *maze)
{
    const char *parse_end;
    cJSON *json = cJSON_ParseWithOpts(raw_json, &parse_end, cJSON_True);
    int status = SUCCESS;

    if (json == NULL) {
        if (parse_end != NULL) {
            printf("Error parsing JSON string at:%s\n", parse_end);
        }
        status = STRING_PARSE_ERR;
        goto parsing_error;
    }

    if ((status = get_safe_int(json, "rows", &maze->rows)) != SUCCESS ||
        (status = get_safe_int(json, "cols", &maze->cols)) != SUCCESS) {
        goto parsing_error;
    }
    // TODO: read the number of walls, player_start coordinates, enemy_start
    // coodinates, escape_pos coordinates and the walls

    printf("status = %d\n maze->rows = %d\n maze->cols = %d\n", status,
           maze->rows, maze->cols);
    return SUCCESS;

parsing_error:
    cJSON_Delete(json);
    return status;
}
