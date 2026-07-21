#ifndef DISPLAY_MAZE
#include "solver.h"
#include "parse.h"

void show_winning_steps(int path_len, game_state queue_arr[], int path_idx[],
                   int escape_r, int escape_c);

void play_winning_sequence(maze_struct *maze);

#endif // !DISPLAY_MAZE
