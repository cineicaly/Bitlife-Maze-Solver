CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic

SRCS = src/main.c

main.exe: $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f main.exe
