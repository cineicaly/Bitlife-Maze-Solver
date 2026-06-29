#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG
#ifdef DEBUG
#define DEBUG_TEST 1
#else
#define DEBUG_TEST 0
#endif

void run(char *file_name)
{ // sizeof(char) is always the smallest
  // addressable unit (i.e. usually 1 byte)

    size_t size =
        sizeof("./res/") + strlen(file_name); // argv[] is guaranteed to have a
                                              // NULL terminator somewhere
    char *file_path = malloc(size);

    if (DEBUG_TEST)
        printf("%zu\n", size);

    sprintf(file_path, "./res/%s", file_name);

    if (DEBUG_TEST)
        printf("%s\n", file_path);
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
