#include "parse.h"
#include "cJSON.h"
#include <stdio.h>
#include <stdlib.h>

void print_maze(maze_struct *maze)
{
    printf("rows = %d\n", maze->rows);
    printf("cols = %d\n", maze->cols);
    printf("num walls = %d\n", maze->num_walls);

    printf("player start coords = %d, %d\n", maze->player_start_ROW,
           maze->player_start_COL);
    printf("enemy start coords = %d, %d\n", maze->enemy_start_ROW,
           maze->enemy_start_COL);
    printf("escape coords = %d, %d\n", maze->escape_ROW,
           maze->escape_COL);

    for (int i = 0; i < maze->num_walls; i++) {
        printf("wall[%d] between (%d, %d) and (%d, %d)\n", i,
               maze->walls[i].ROW_1, maze->walls[i].COL_1, maze->walls[i].ROW_2,
               maze->walls[i].COL_2);
    }
}

int get_safe_size(cJSON *json, const char *key, int *rows_cols)
{
    cJSON *item = cJSON_GetObjectItemCaseSensitive(json, key);
    if (cJSON_IsNumber(item)) {
        *rows_cols = item->valueint;
        return SUCCESS;
    }
    printf("Error: '%s' is missing or not a valid number.\n", key);
    return INT_READ_ERR;
}

int get_safe_coord(cJSON *json, const char *key, int *out_x, int *out_y)
{
    cJSON *item = cJSON_GetObjectItemCaseSensitive(json, key);
    if (cJSON_IsArray(item) && cJSON_GetArraySize(item) == 2) {
        cJSON *x = cJSON_GetArrayItem(item, 0);
        cJSON *y = cJSON_GetArrayItem(item, 1);

        if (cJSON_IsNumber(x) && cJSON_IsNumber(y)) {
            *out_x = x->valueint;
            *out_y = y->valueint;
            return SUCCESS;
        } else {
            printf("Error: '%s' is not a valid number (coordinate).\n", key);
            return COORD_INT_READ_ERR;
        }
    } else {
        printf("Error: '%s' is missing, not valid array or not a valid array "
               "size.\n",
               key);
        return COORD_ARR_ERR;
    }
}

int get_safe_walls(cJSON *json, const char *key, int *num_walls,
                   Wall **walls_ptr)
{
    cJSON *walls_arr = cJSON_GetObjectItemCaseSensitive(json, key);
    if (!cJSON_IsArray(walls_arr)) {
        printf("Error: '%s' is missing or not an array\n.", key);
        return WALL_ARR_ERR;
    } else {
        /* READ BIG ARRAY */
        *num_walls = cJSON_GetArraySize(walls_arr);
        if (*num_walls != 0) { // prevent malloc(0)
            *walls_ptr = malloc(*num_walls * sizeof(Wall));
            if (*walls_ptr == NULL) {
                return ALLOC_ERR;
            }
        } else {
            printf("Error: '%s' is an array of size 0\n.", key);
            return WALL_ARR_SIZE_0_ERR;
        }
        /*READ THE ARRAY OF POINT-PAIRS*/
        for (int i = 0; i < *num_walls; i++) {
            // walk through the pairs
            cJSON *wall = cJSON_GetArrayItem(walls_arr, i);
            if (!cJSON_IsArray(wall)) {
                printf("Error: Wall entry at index %d is not a valid array.\n",
                       i);
                return WALL_ARR_ERR;
            }
            if (cJSON_GetArraySize(wall) != 2) {
                printf("Error: Number of points =/= 2 at wall index %i.\n", i);
                return WALL_ARR_COUNT_ERR;
            }

            // get the pairs of wall coordinates
            cJSON *p1 = cJSON_GetArrayItem(wall, 0);
            cJSON *p2 = cJSON_GetArrayItem(wall, 1);

            if (!cJSON_IsArray(p1) || !cJSON_IsArray(p2)) {
                printf(
                    "Error: At least one of the pairs of coordinate arrays of "
                    "the wall-adjacent tiles at index %i are not valid "
                    "arrays of "
                    "coordinates.\n",
                    i);
                return WALL_COORD_ARR_ERR;
            }
            if (cJSON_GetArraySize(p1) != 2 || cJSON_GetArraySize(p2) != 2) {
                printf("Error: The size of one or both of the array/arrays "
                       "that hold the coordinates of the tiles adjacent to the "
                       "wall at index %i are =/= 2.\n",
                       i);
                return WALL_COORD_COUNT_ERR;
            }

            cJSON *p1_x = cJSON_GetArrayItem(p1, 0);
            cJSON *p1_y = cJSON_GetArrayItem(p1, 1);

            cJSON *p2_x = cJSON_GetArrayItem(p2, 0);
            cJSON *p2_y = cJSON_GetArrayItem(p2, 1);

            if (!cJSON_IsNumber(p1_x) || !cJSON_IsNumber(p1_y) ||
                !cJSON_IsNumber(p2_x) || !cJSON_IsNumber(p2_y)) {
                printf("Error: At least one of coordinates of the tiles "
                       "adjacent to the "
                       "wall at index %i are not numbers.\n",
                       i);
                return WALL_COORD_NAN_ERR;
            }
            (*walls_ptr)[i].ROW_1 = p1_x->valueint;
            (*walls_ptr)[i].COL_1 = p1_y->valueint;
            (*walls_ptr)[i].ROW_2 = p2_x->valueint;
            (*walls_ptr)[i].COL_2 = p2_y->valueint;
        }
    }
    return SUCCESS;
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

    if ((status = get_safe_size(json, "rows", &maze->rows)) != SUCCESS ||
        (status = get_safe_size(json, "cols", &maze->cols)) != SUCCESS) {
        goto parsing_error;
    }

    if ((status = get_safe_coord(json, "player_start", &maze->player_start_ROW,
                                 &maze->player_start_COL)) != SUCCESS ||
        (status = get_safe_coord(json, "enemy_start", &maze->enemy_start_ROW,
                                 &maze->enemy_start_COL)) != SUCCESS ||
        (status = get_safe_coord(json, "escape_pos", &maze->escape_ROW,
                                 &maze->escape_COL)) != SUCCESS) {
        goto parsing_error;
    }

    // Get number of walls
    if ((status = get_safe_walls(json, "walls", &maze->num_walls,
                                 &maze->walls)) != SUCCESS) {
        goto parsing_error;
    }

    print_maze(maze);
    return SUCCESS;

parsing_error:
    cJSON_Delete(json);
    return status;
}
