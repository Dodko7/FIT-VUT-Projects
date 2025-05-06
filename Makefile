# Makefile for C++17 backend with nlohmann/json

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Ilib/single_include -Ilib/include/single_include
SRC_DIR = src/core
OBJ_DIR = build
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))
TARGET = core_backend

.PHONY: all clean directories build-test

all: directories $(TARGET)

$(TARGET): $(OBJS)
    $(CXX) $(CXXFLAGS) -o $@ $(OBJS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | directories
    $(CXX) $(CXXFLAGS) -c $< -o $@

directories:
    @mkdir -p $(OBJ_DIR)

clean:
    rm -rf $(OBJ_DIR) $(TARGET)

build-test:
	$(CXX) $(CXXFLAGS) -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-unused-but-set-variable -o $(TARGET)-test $(SRCS)