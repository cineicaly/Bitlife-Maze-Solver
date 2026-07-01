#include "solver.h"
#include "parse.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void display_moves(int path_len, game_state queue_arr[], int path_idx[],
                   int escape_r, int escape_c);

bool check_wall(int curr_r, int curr_c, int target_r, int target_c,
                maze_struct *maze, int idx)
{
    return (curr_r == maze->walls[idx].ROW_1 &&
            curr_c == maze->walls[idx].COL_1 &&
            target_r == maze->walls[idx].ROW_2 &&
            target_c == maze->walls[idx].COL_2) ||
           (curr_r == maze->walls[idx].ROW_2 &&
            curr_c == maze->walls[idx].COL_2 &&
            target_r == maze->walls[idx].ROW_1 &&
            target_c == maze->walls[idx].COL_1);
}

bool can_move(int curr_r, int curr_c, int target_r, int target_c,
              maze_struct *maze)
{
    // Check if target is escape tile.
    bool is_escape =
        (target_r == maze->escape_ROW && target_c == maze->escape_COL);

    // All other edge tiles beside the escape tile are walled off.
    if (!is_escape) {
        if (target_r <= 0 || target_r >= maze->rows - 1 || target_c <= 0 ||
            target_c >= maze->cols - 1) {
            return false;
        }
    }

    // Internal wall check.
    for (int i = 0; i < maze->num_walls; i++)
        if (check_wall(curr_r, curr_c, target_r, target_c, maze, i)) {
            return false;
        }

    return true;
}

game_state simulate_guard(game_state state, maze_struct *maze)
{
    for (int step = 0; step < 2; step++) {
        if (state.player_r == state.enemy_r &&
            state.player_c == state.enemy_c) {
            return state;
        }

        bool moved_this_step = false;

        if (state.enemy_c != state.player_c) {
            int dir_c = (state.enemy_c > state.player_c) ? -1 : 1;

            if (can_move(state.enemy_r, state.enemy_c, state.enemy_r,
                         state.enemy_c + dir_c, maze)) {
                state.enemy_c += dir_c;
                moved_this_step = true;
            }
        }
        if (moved_this_step == false && state.enemy_r != state.player_r) {
            int dir_r = (state.enemy_r > state.player_r) ? -1 : 1;

            if (can_move(state.enemy_r, state.enemy_c, state.enemy_r + dir_r,
                         state.enemy_c, maze)) {
                state.enemy_r += dir_r;
                moved_this_step = true;
            }
        }
    }

    return state;
}

void solve(maze_struct *maze, int *steps)
{
    if (maze->player_start_ROW == maze->enemy_start_ROW &&
        maze->player_start_COL == maze->enemy_start_COL) {
        printf("Enemy started ontop of player.\n");
        return;
    }

    int rows = maze->rows;
    int cols = maze->cols;

    if (rows > 160 || cols > 160) {
        printf("Maze is too big. Max size: r = 160 && c = 160\n"
               "Yours is: r = %d, c = %d\n",
               rows, cols);
        return;
    }
    int MAX_STATES = (rows * cols) * (rows * cols);
    game_state *queue_arr = malloc(sizeof(game_state) * MAX_STATES);
    if (!queue_arr) {
        printf("Failed to allocate memory for queue_arr\n");
        return;
    }

    bool (*visited)[maze->cols][maze->rows][maze->cols] =
        calloc(maze->rows, sizeof(bool[maze->cols][maze->rows][maze->cols]));
    if (!visited) {
        printf("Failed to allocate memory for the visited 4D array.\n");
        free(queue_arr);
        return;
    }

    int *path_idx = malloc(MAX_STATES * sizeof(int));
    if (!path_idx) {
        printf("Failed to allocate memory for path_idx.\n");
        free(queue_arr);
        free(visited);
        return;
    }

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
            while (curr_idx != -1) {
                path_idx[path_len] = curr_idx;
                path_len++;
                curr_idx = queue_arr[curr_idx].parent_idx;
            }

            *steps = path_len;
            // TODO: display_path()
            display_moves(path_len, queue_arr, path_idx, escape_r, escape_c);

            break;
        } else {
            int move_r[4] = {-1, 1, 0, 0};
            int move_c[4] = {0, 0, -1, 1};
            for (int i = 0; i < 4; i++) {
                next_state = curr_state;
                next_state.player_r += move_r[i];
                next_state.player_c += move_c[i];

                if (!can_move(curr_state.player_r, curr_state.player_c,
                              next_state.player_r, next_state.player_c, maze)) {
                    continue;
                }
                if (next_state.player_r != escape_r ||
                    next_state.player_c != escape_c) {

                    next_state = simulate_guard(next_state, maze);

                    if (next_state.player_r == next_state.enemy_r &&
                        next_state.player_c == next_state.enemy_c) {
                        continue;
                    }
                }

                if (visited[next_state.player_r][next_state.player_c]
                           [next_state.enemy_r][next_state.enemy_c]) {
                    continue;
                }

                visited[next_state.player_r][next_state.player_c]
                       [next_state.enemy_r][next_state.enemy_c] = true;

                next_state.parent_idx = head - 1;
                queue_arr[tail] = next_state;
                tail++;
            }
        }
    }

    free(visited);
    free(queue_arr);
    free(path_idx);
    return;
}

void display_moves(int path_len, game_state queue_arr[], int path_idx[],
                   int escape_r, int escape_c)
{
    printf("\nWIN FOUND IN %d MOVES: \n", path_len);
    for (int i = path_len - 1; i > 0; i--) {
        game_state current_step = queue_arr[path_idx[i]];
        game_state next_step = queue_arr[path_idx[i - 1]];

        if ((path_len - i) % 10 == 0)
            printf("\n");
        if (current_step.player_r > next_step.player_r)
            printf("Up->");
        else if (current_step.player_r < next_step.player_r)
            printf("Down->");
        else if (current_step.player_c > next_step.player_c)
            printf("Left->");
        else if (current_step.player_c < next_step.player_c)
            printf("Right->");
    }
    printf("(%d, %d)!\n\n", escape_r, escape_c);
}
