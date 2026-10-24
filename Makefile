CC ?= gcc
CFLAGS ?= -std=c11 -Wall -Wextra -pedantic -O2
TARGET = lost_found
SRC = lost_found.c

.PHONY: all run clean
all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) $(TARGET).exe
