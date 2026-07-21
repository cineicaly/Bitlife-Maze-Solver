
#include "parse.h"
#include "solver.h"
#include <stdio.h>
#include <stdlib.h>

void show_winning_steps(int path_len, game_state queue_arr[], int path_idx[],
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

void print_game_state(int rows, int cols, int matrix_maze[rows][cols])
{
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%c", matrix_maze[i][j]);
        }
        printf("\n");
    }
}

void play_winning_sequence(maze_struct *maze)
{
    int rows = maze->rows * 2 + 1;
    int cols = maze->cols * 2 + 1;

    int (*matrix_maze)[cols] = malloc(sizeof(*matrix_maze) * rows);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if ((i == 2 && j > 2 && j < (cols - 3)) ||
                (i == (rows - 3) && j > 2 && j < (cols - 3)) ||
                (j == 2 && i > 2 && i < rows - 2) ||
                (j == (cols - 3) && i > 2 && i < (rows - 3)))
                matrix_maze[i][j] = '#';
            else
                matrix_maze[i][j] = ' ';
        }
    }

    for (int wall = 0; wall < maze->num_walls; wall++) {
        matrix_maze[maze->walls[wall].ROW_1 + maze->walls[wall].ROW_2 + 1]
                   [maze->walls[wall].COL_1 + maze->walls[wall].COL_2 + 1] =
                       '#';
    }
    int corner = 0;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (i > 2 && i < (rows - 3) && j > 2 && j < (cols - 3)) {
                if ((i % 2 == 0) && (j % 2 == 0)) {
                    matrix_maze[i][j] = '.';

                    if (matrix_maze[i + 1][j] == '#')
                        corner++;
                    if (matrix_maze[i - 1][j] == '#')
                        corner++;
                    if (matrix_maze[i][j + 1] == '#')
                        corner++;
                    if (matrix_maze[i][j - 1] == '#')
                        corner++;

                    if (corner >= 2) {
                        matrix_maze[i][j] = '#';
                    }
                    corner = 0;
                }
            }
        }
    }

    matrix_maze[maze->player_start_ROW * 2 + 1]
               [maze->player_start_COL * 2 + 1] = 'P';
    matrix_maze[maze->enemy_start_ROW * 2 + 1][maze->enemy_start_COL * 2 + 1] =
        'C';

    print_game_state(rows, cols, matrix_maze);

    free(matrix_maze);
}
