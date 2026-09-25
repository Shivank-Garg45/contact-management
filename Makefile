CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c99
TARGET = contact_manager

SRCS = main.c auth.c contact.c groups.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) app.exe

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
