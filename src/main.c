#include "../lib/parse.h"
#include "../lib/solver.h"
#include <bits/time.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#undef DEBUG
#ifdef DEBUG
#define DEBUG_TEST 1
#else
#define DEBUG_TEST 0
#endif

void print_error_msg(int status)
{
    if (status == SUCCESS) {
        printf("PARSING JSON WAS SUCCESSFUL\n");
        return;
    }

    printf("ERR CODE: ");
    switch (status) {
    case STRING_PARSE_ERR:
        printf("STRING_PARSE_ERR\n");
        break;
    case INT_READ_ERR:
        printf("INT_READ_ERR\n");
        break;
    case COORD_INT_READ_ERR:
        printf("COORD_INT_READ_ERR\n");
        break;
    case COORD_ARR_ERR:
        printf("COORD_ARR_ERR\n");
        break;
    case WALL_ARR_ERR:
        printf("WALL_ARR_ERR\n");
        break;
    case WALL_ARR_SIZE_0_ERR:
        printf("WALL_ARR_SIZE_0_ERR\n");
        break;
    case WALL_ARR_COUNT_ERR:
        printf("WALL_ARR_COUNT_ERR\n");
        break;
    case WALL_COORD_ARR_ERR:
        printf("WALL_COORD_ARR_ERR\n");
        break;
    case WALL_COORD_COUNT_ERR:
        printf("WALL_COORD_COUNT_ERR\n");
        break;
    case WALL_COORD_NAN_ERR:
        printf("WALL_COORD_NAN_ERR\n");
        break;
    }
}

char *read_file(const char *file_path)
{
    FILE *fp = fopen(file_path, "rb");
    if (!fp) {
        perror("Open JSON");
        exit(EXIT_FAILURE);
    }

    if (fseek(fp, 0, SEEK_END)) {
        perror("couldn't read file: ");
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    long file_size = ftell(fp);
    if (file_size == -1L) {
        perror("couldn't read file size: ");
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    rewind(fp);

    if (DEBUG_TEST)
        printf("FILE SIZE = %ld\n", file_size);

    char *input = malloc(file_size + 1);
    if (!input) {
        perror("failed to alloc memory: ");
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    size_t read_len = fread(input, 1, file_size, fp);

    if (DEBUG_TEST)
        printf("FREAD READ SIZE = %zu\n", read_len);

    if (read_len != (size_t)file_size) { // fread() doesn't set errno, have to
                                         // check for ferror() & feof()
        if (ferror(fp))
            printf("error reading %s\n", file_path);
        else if (feof(fp)) {
            printf("EOF found\n");
            printf("number of characters read = %zu\n", read_len);
            printf("input = %.*s\n", (int)read_len, input);
        }
        free(input);
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    input[read_len] = '\0';
    fclose(fp);

    return input;
}

void run(char *file_name)
{ // sizeof(char) is always the smallest
  // addressable unit (i.e. usually 1 byte)

    size_t size =
        sizeof("./res/") + strlen(file_name); // argv[] is guaranteed to have a
                                              // NULL terminator somewhere
    char *file_path = malloc(size);

    if (DEBUG_TEST)
        printf("path size: %zu\n", size);

    sprintf(file_path, "./res/%s", file_name);

    if (DEBUG_TEST)
        printf("file_path: %s\n", file_path);

    char *raw_json = read_file(file_path);
    if (DEBUG_TEST)
        printf("%s\n", raw_json);

    maze_struct *maze = malloc(sizeof(maze_struct));
    maze->walls = NULL;
    maze->num_walls = 0;

    int status;
    status = parse_json(raw_json, maze);
    print_error_msg(status);
    if (status == SUCCESS && DEBUG_TEST) {
        struct timespec start, end;
        int steps_ = -1;

        clock_gettime(CLOCK_MONOTONIC, &start);

        solve(maze, &steps_);

        clock_gettime(CLOCK_MONOTONIC, &end);
        double time_taken = (end.tv_sec - start.tv_sec) * 1e9;
        time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9;
        printf("\nSolved in %d steps.\n Solver execution time: %.9f seconds\n",
               steps_, time_taken);
    }

    int steps = -1;
    if (status == SUCCESS && !DEBUG_TEST) {
        solve(maze, &steps);
        if (steps == -1) {
            printf("No solution was found.\n");
        }
    }

    free(raw_json);
    // if (DEBUG_TEST)
    //     printf("freeing raw_json worked\n");
    free(maze->walls);
    // if (DEBUG_TEST)
    //     printf("freeing maze->walls worked\n");
    free(maze);
    // if (DEBUG_TEST)
    //     printf("freeing maze worked\n");

    free(file_path);
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("usage: ./main.exe <file_name>\n");
        exit(EXIT_FAILURE);
    }

    run(argv[1]);

    return EXIT_SUCCESS;
}
