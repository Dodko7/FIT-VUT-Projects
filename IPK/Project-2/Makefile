# Compiler to use
CC = gcc

# Compiler flags: C17 standard, all warnings enabled, debug info, POSIX compatibility
CFLAGS = -std=c17 -Wall -Wextra -D_DEFAULT_SOURCE -g

# Linker flags: link against libpcap for packet capturing
LDFLAGS = -lpcap

# Object files to build (all located in src/)
OBJ = src/args.o src/utils.o src/packet.o src/capture.o src/scanner.o src/main.o

# Target executable name
TARGET = ipk-l4-scan

# Default target: build the executable
all: $(TARGET)

# Link object files into the final executable
$(TARGET): $(OBJ)
		$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

# Compile args.c to args.o (depends on args.h and utils.h for print_interfaces)
src/args.o: src/args.c src/args.h src/utils.h
		$(CC) $(CFLAGS) -c src/args.c -o src/args.o

# Compile utils.c to utils.o (depends on utils.h)
src/utils.o: src/utils.c src/utils.h
		$(CC) $(CFLAGS) -c src/utils.c -o src/utils.o

# Compile packet.c to packet.o (depends on packet.h and utils.h for checksum)
src/packet.o: src/packet.c src/packet.h src/utils.h
		$(CC) $(CFLAGS) -c src/packet.c -o src/packet.o

# Compile capture.c to capture.o (depends on scanner.h after merging capture.h)
src/capture.o: src/capture.c src/scanner.h
		$(CC) $(CFLAGS) -c src/capture.c -o src/capture.o

# Compile scanner.c to scanner.o (depends on scanner.h, utils.h, packet.h, args.h)
src/scanner.o: src/scanner.c src/scanner.h src/utils.h src/packet.h src/args.h
		$(CC) $(CFLAGS) -c src/scanner.c -o src/scanner.o

# Compile main.c to main.o (depends on args.h and scanner.h)
src/main.o: src/main.c src/args.h src/scanner.h
		$(CC) $(CFLAGS) -c src/main.c -o src/main.o

# Clean up object files and executable
clean:
		rm -f $(OBJ) $(TARGET)

# Phony targets (not actual files)
.PHONY: all clean