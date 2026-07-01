#ifndef SOLVER_H
#define SOLVER_H

#include "parse.h"
typedef struct game_state {
    int player_r, player_c;
    int enemy_r, enemy_c;
    int parent_idx;
} game_state;

void solve(maze_struct *maze);

#endif
