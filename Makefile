CC = gcc

CFLAGS = -Wall -Wextra 
LDFLAGS = -lSDL2

TARGET = lc3vm

SRCS = lc3.c utils.c instructions.c
HEADERS = lc3.h

OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

asm:
	./lc3as flappy.asm

play:
	./$(TARGET) flappy.obj
