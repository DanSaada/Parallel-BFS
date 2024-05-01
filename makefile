# Target executable name
TARGET = part_III.out

# Source files
SRCS = main.c BFS/bfs.c SyncGraph/graph.c ThreadPool/TaskQueue.c ThreadPool/ThreadPool.c

# Compiler and flags
CC = gcc
# CFLAGS = place flags if needed

# Default target builds the executable
$(TARGET): $(SRCS)
	$(CC) $(SRCS) $(CFLAGS) -o $@

# Clean target removes the executable
clean:
	rm -f $(TARGET)
