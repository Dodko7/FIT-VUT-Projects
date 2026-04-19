# Makefile pre C++17 backend s nlohmann/json, Catch2 a Qt5

# Kompilátor
CXX = clang++
CXXFLAGS = -std=gnu++2b -Wall -Wextra -Ilib/nlohmann/single_include -Ilib/catch2 -Isrc/core -w
MOC = /opt/homebrew/opt/qt@5/bin/moc

# Qt konfigurácia
QT_PREFIX = /opt/homebrew/Cellar/qt@5/5.15.16_2
CXXFLAGS += -I$(QT_PREFIX)/include -I$(QT_PREFIX)/include/QtCore -I$(QT_PREFIX)/include/QtScript
LDFLAGS = -F$(QT_PREFIX)/lib -framework QtCore -framework QtScript
CXXFLAGS += -DQT_NO_DEBUG -DQT_NO_KEYWORDS
CPPFLAGS = -I$(QT_PREFIX)/include
export PKG_CONFIG_PATH = $(QT_PREFIX)/lib/pkgconfig

# Create a separate variable for moc flags
MOCFLAGS = -I/opt/homebrew/opt/qt@5/include \
		   -I/opt/homebrew/opt/qt@5/include/QtCore \
		   -I/opt/homebrew/opt/qt@5/include/QtScript \
		   -Isrc/core \
		   -DQT_NO_DEBUG \
		   -DQT_NO_KEYWORDS

# Adresáre a súbory
SRC_DIR = src/core
TEST_DIR = tests
EXAMPLES_DIR = examples
OBJ_DIR = build
TEST_OBJ_DIR = build/tests
EXAMPLES_OBJ_DIR = build/examples
SRCS = $(filter-out $(SRC_DIR)/main.cpp, $(wildcard $(SRC_DIR)/*.cpp))
TEST_SRCS = $(wildcard $(TEST_DIR)/*.cpp)
EXAMPLE_SRCS = $(wildcard $(EXAMPLES_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))
TEST_OBJS = $(patsubst $(TEST_DIR)/%.cpp,$(TEST_OBJ_DIR)/%.o,$(TEST_SRCS))
EXAMPLE_OBJS = $(patsubst $(EXAMPLES_DIR)/%.cpp,$(EXAMPLES_OBJ_DIR)/%.o,$(EXAMPLE_SRCS))
EXAMPLE_TARGETS = $(patsubst $(EXAMPLES_DIR)/%.cpp,$(EXAMPLES_OBJ_DIR)/%,$(EXAMPLE_SRCS))
MOC_SRC = $(SRC_DIR)/scriptEngine.hpp
MOC_OBJ = $(OBJ_DIR)/moc_scriptEngine.o
TARGET = core_backend
TEST_TARGET = test_suite
FSM_TEST_RUNNER = fsm_test_runner

.PHONY: all clean directories test examples tests_only run_tests run_fsm_tests build_all

# Default target just builds the core program
all: directories $(TARGET)

# Separate target for building everything
build_all: directories $(TARGET) examples test

examples: directories $(EXAMPLE_TARGETS)

# Target to compile only the tests without running them
tests_only: directories $(TEST_TARGET)

# Target to run the tests after compiling them
run_tests: tests_only
	./$(TEST_TARGET) -r junit -o test-results.xml || true
	./$(TEST_TARGET) -r console

# Target to compile and run the new FSM test runner
run_fsm_tests: directories $(FSM_TEST_RUNNER)
	./$(FSM_TEST_RUNNER) || true

$(TARGET): $(OBJ_DIR)/main.o $(OBJS) $(MOC_OBJ)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | directories
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

$(OBJ_DIR)/moc_%.o: $(OBJ_DIR)/moc_%.cpp | directories
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

$(OBJ_DIR)/moc_%.cpp: $(SRC_DIR)/%.hpp
	$(MOC) $(MOCFLAGS) $< -o $@

$(TEST_OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp | directories
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

# Update the test target to use our FSM test runner
test: run_fsm_tests

$(TEST_TARGET): $(TEST_OBJS) $(OBJS) $(MOC_OBJ)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

# Rule to compile the FSM test runner
$(FSM_TEST_RUNNER): $(TEST_OBJ_DIR)/fsm_test_runner.o $(OBJS) $(MOC_OBJ)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

# Rule to compile example object files
$(EXAMPLES_OBJ_DIR)/%.o: $(EXAMPLES_DIR)/%.cpp | directories
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

# Rule to link example executables
$(EXAMPLES_OBJ_DIR)/%: $(EXAMPLES_OBJ_DIR)/%.o $(OBJS) $(MOC_OBJ) | directories
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

directories:
	@mkdir -p $(OBJ_DIR) $(TEST_OBJ_DIR) $(EXAMPLES_OBJ_DIR) assets examples

clean:
	rm -rf $(OBJ_DIR) $(TEST_OBJ_DIR) $(EXAMPLES_OBJ_DIR) assets $(TARGET) $(TEST_TARGET) $(FSM_TEST_RUNNER) $(EXAMPLE_TARGETS)