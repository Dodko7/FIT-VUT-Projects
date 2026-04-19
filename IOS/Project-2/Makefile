CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra -Werror -pedantic

SRCS = proj2.c processes.c
OBJS = $(SRCS:.c=.o)

TARGET = proj2

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $^ -o $@ -lrt -pthread

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)