# =========================================
# ISA DNS Filter - Makefile 
# =========================================

# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -O2 -Iinclude
SRC_DIR = src

# Object files (explicit list for BSD make)
OBJ = $(SRC_DIR)/addr_resolver.o $(SRC_DIR)/arg_parser.o $(SRC_DIR)/dns_compression.o \
      $(SRC_DIR)/dns_protocol.o $(SRC_DIR)/dns_response.o $(SRC_DIR)/domain_utils.o \
      $(SRC_DIR)/filter.o $(SRC_DIR)/id_mapper.o $(SRC_DIR)/logger.o \
      $(SRC_DIR)/main.o $(SRC_DIR)/query_handler.o $(SRC_DIR)/server.o \
      $(SRC_DIR)/socket_manager.o

TARGET = dns

# Default target
all: $(TARGET)

# Link target
$(TARGET): $(OBJ)
	@echo "Linking $@ ..."
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ)

# Compile source files
$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "Compiling $< ..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	@echo "Cleaning..."
	rm -f $(TARGET) $(OBJ)
	@find . -type f -name "*.pyc" -delete 2>/dev/null || true
	@find . -type d -name "__pycache__" -exec rm -rf {} + 2>/dev/null || true

# Run example
run: $(TARGET)
	sudo ./$(TARGET) -p 53 -s 8.8.8.8 -f filter.txt

# Run tests
test: $(TARGET)
	python3 tests/tests.py

# Pack project for submission
pack:
	tar -cvf xondre16.tar Makefile src include tests manual.pdf README
	@echo "Created xondre16.tar (uncompressed)"

# Phony targets
.PHONY: all clean run test pack
