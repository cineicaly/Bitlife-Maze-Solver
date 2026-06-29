#ifndef PARSE_H
#define PARSE_H

enum PARSE_ERROR { SUCCESS, STRING_PARSE_ERR, INT_READ_ERR };

typedef struct {
    int point_A[2];
    int point_B[2];
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
