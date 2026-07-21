CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -lm

SRCS = src/main.c lib/parse.c lib/cJSON.c lib/solver.c lib/display_maze.c

main.exe: $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f main.exe
