CC      = gcc
CFLAGS  = -Wall -Wextra -O2 -std=c11 -Iinclude
SRC     = src/main.c src/kbhit.c src/timectl.c src/display.c
OBJ     = $(SRC:.c=.o)
TARGET  = santrancsaat

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
