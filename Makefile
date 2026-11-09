CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2
TARGET = 4enratlla.exe
SRCS = 4enratlla.c minimax.c
HEADERS = minimax.h

all: $(TARGET)

$(TARGET): $(SRCS) $(HEADERS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)                                                                            

clean:
	del /f $(TARGET) *.o