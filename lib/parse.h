#ifndef PARSE_H
#define PARSE_H

enum PARSE_ERROR {
    SUCCESS,
    STRING_PARSE_ERR,
    INT_READ_ERR,
    COORD_INT_READ_ERR,
    COORD_ARR_ERR,
    WALL_ARR_ERR,
    WALL_ARR_SIZE_0_ERR,
    WALL_ARR_COUNT_ERR,
    WALL_COORD_ARR_ERR,
    WALL_COORD_COUNT_ERR,
    WALL_COORD_NAN_ERR,
    ALLOC_ERR
};

typedef struct {
    int x_1, y_1;
    int x_2, y_2;
} Wall;

typedef struct maze_struct {
    int rows;
    int cols;
    int num_walls;

    int player_start[2];
    int enemy_start[2];
    int escape_pos[2];

    Wall *walls;
} maze_struct;

int parse_json(const char *raw_json, maze_struct *maze);

#endif
