CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2
TARGET = 4enratlla.exe
SRCS = 4enratlla.c minimax.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)                                                                            

clean:
	del /f $(TARGET) *.o