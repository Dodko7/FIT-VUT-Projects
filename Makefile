# Makefile pre C++17 backend s nlohmann/json, Catch2 a Qt5

# Kompilátor
CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -Ilib/nlohmann/single_include -Ilib/catch2 -Isrc/core -w
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
OBJ_DIR = build
TEST_OBJ_DIR = build/tests
SRCS = $(filter-out $(SRC_DIR)/main.cpp, $(wildcard $(SRC_DIR)/*.cpp))
TEST_SRCS = $(filter-out $(TEST_DIR)/test_main.cpp, $(wildcard $(TEST_DIR)/*.cpp))
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))
TEST_OBJS = $(TEST_OBJ_DIR)/test_main.o $(patsubst $(TEST_DIR)/%.cpp,$(TEST_OBJ_DIR)/%.o,$(TEST_SRCS))
MOC_SRC = $(SRC_DIR)/script_engine.hpp
MOC_OBJ = $(OBJ_DIR)/moc_script_engine.o
TARGET = core_backend
TEST_TARGET = test_suite

.PHONY: all clean directories test

all: directories $(TARGET)

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

test: directories $(TEST_TARGET)
	./$(TEST_TARGET) -r junit -o test-results.xml || true
	./$(TEST_TARGET) -r console

$(TEST_TARGET): $(TEST_OBJS) $(OBJS) $(MOC_OBJ)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

directories:
	@mkdir -p $(OBJ_DIR) $(TEST_OBJ_DIR) assets examples

clean:
	rm -rf $(OBJ_DIR) $(TEST_OBJ_DIR) assets $(TARGET) $(TEST_TARGET)