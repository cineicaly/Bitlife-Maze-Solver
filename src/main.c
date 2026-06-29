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

char *read_file(char *file_path)
{
    FILE *fp = fopen(file_path, "r");
    if (!fp) {
        perror("open: ");
        exit(EXIT_FAILURE);
    }

    if (fseek(fp, 0, SEEK_END)) {
        perror("couldn't read file: ");
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    long file_size = ftell(fp);
    if (file_size == -1) {
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

    int read_len = fread(input, 1, file_size, fp);

    if (DEBUG_TEST)
        printf("FREAD READ SIZE = %d\n", read_len);

    if (read_len != file_size) {
        if (ferror(fp))
            printf("error reading %s\n", file_path);
        else if (feof(fp)) {
            printf("EOF found\n");
            printf("number of characters read = %d\n", read_len);
            printf("input = %.*s\n", read_len, input);
        }
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    input[file_size] = '\0';
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

    printf("%s\n", read_file(file_path));
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
