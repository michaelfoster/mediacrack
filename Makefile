CFLAGS = -I. -std=c99 -g -D_GNU_SOURCE -Wall

SRCS = mediacrack.c

OBJS = $(SRCS:.c=.o)
LIBS = -lcrypto -lpthread
BIN = mediacrack

CC=gcc

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(LIBS) -o $(BIN) $(OBJS)

clean:
	$(RM) $(BIN) $(OBJS)

