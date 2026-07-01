#include "solver.h"
#include "parse.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

bool can_move(int curr_r, int curr_c, int target_r, int target_c,
              maze_struct *maze)
{
    return true;
}
game_state simulate_guard(game_state target, maze_struct *maze)
{
    return target;
}

void solve(maze_struct *maze)
{
    if (maze->player_start_ROW == maze->enemy_start_ROW &&
        maze->player_start_COL == maze->enemy_start_ROW) {
        printf("Enemy started ontop of player.\n");
        return;
    }

    int rows = maze->rows;
    int cols = maze->cols;

    if (rows > 20 || cols > 20) {
        printf("Maze is too big. Max size: r = 20 && c = 20\n"
               "Yours is: r = %d, c = %d\n",
               rows, cols);
        return;
    }

    int MAX_STATES = (rows * cols) * (rows * cols);
    game_state *queue_arr = malloc(sizeof(game_state) * MAX_STATES);
    bool ****visited = calloc(MAX_STATES, sizeof(bool));

    game_state curr_state;
    curr_state.player_r = maze->player_start_ROW;
    curr_state.player_c = maze->player_start_COL;
    curr_state.enemy_r = maze->enemy_start_ROW;
    curr_state.enemy_c = maze->enemy_start_COL;
    curr_state.parent_idx = -1;

    int escape_r = maze->escape_ROW;
    int escape_c = maze->escape_COL;

    int head = 0, tail = 0;

    queue_arr[tail] = curr_state;
    tail++;

    visited[curr_state.player_r][curr_state.player_c][curr_state.enemy_r]
           [curr_state.enemy_c] = true;
    game_state next_state;
    while (head < tail) {
        curr_state = queue_arr[head];
        head++;

        if (curr_state.player_c == escape_c &&
            curr_state.player_r == escape_r) {
            int curr_idx = head - 1;
            int path_len = 0;
            int path_idx[MAX_STATES];
            while (curr_idx != -1) {
                path_idx[path_len] = curr_idx;
                path_len++;
                curr_idx = queue_arr[curr_idx].parent_idx;
            }
            // TODO: display_moves(); display_path()

            break;
        } else {
            int move_r[4] = {-1, 1, 0, 0};
            int move_c[4] = {0, 0, -1, 1};
            for (int i = 0; i < 4; i++) {
                next_state = curr_state;
                next_state.player_r += move_r[i];
                next_state.player_c += move_c[i];

                // TODO: can_move();
                if (can_move(curr_state.player_r, curr_state.player_c,
                             curr_state.enemy_r, curr_state.enemy_c, maze)) {
                    if (next_state.player_r != escape_r ||
                        next_state.player_c != escape_c) {

                        // TODO: simulate_guard();
                        next_state = simulate_guard(next_state, maze);

                        if (next_state.player_r == next_state.enemy_r &&
                            next_state.player_c == next_state.enemy_r) {
                            continue;
                        }
                    }

                    if (visited[next_state.player_r][next_state.player_c]
                               [next_state.enemy_r][next_state.enemy_c] ==
                        false) {
                        visited[next_state.player_r][next_state.player_c]
                               [next_state.enemy_r][next_state.enemy_c] = true;
                        next_state.parent_idx = head - 1;
                        queue_arr[tail] = next_state;
                        tail++;
                    }
                }
            }
        }
    }

    return;
}
