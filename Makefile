# Makefile for C++17 backend with nlohmann/json and Catch2

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Ilib/nlohmann/single_include -Ilib/catch2 -Isrc/core -w
SRC_DIR = src/core
TEST_DIR = tests
OBJ_DIR = build
TEST_OBJ_DIR = build/tests
SRCS = $(filter-out $(SRC_DIR)/main.cpp, $(wildcard $(SRC_DIR)/*.cpp))
TEST_SRCS = $(wildcard $(TEST_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))
TEST_OBJS = $(patsubst $(TEST_DIR)/%.cpp,$(TEST_OBJ_DIR)/%.o,$(TEST_SRCS))
TARGET = core_backend
TEST_TARGET = test_suite

.PHONY: all clean directories test

all: directories $(TARGET)

$(TARGET): $(OBJ_DIR)/main.o $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | directories
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TEST_OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp | directories
	$(CXX) $(CXXFLAGS) -c $< -o $@

test: directories $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(TEST_OBJS) $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

directories:
	@mkdir -p $(OBJ_DIR) $(TEST_OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR) $(TEST_OBJ_DIR) $(TARGET) $(TEST_TARGET)