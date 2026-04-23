# Makefile for CLI build of Nimonspoli
# For GUI build, use: cmake -S . -B build && cmake --build build

CXX      := g++
CXXFLAGS := -Wall -Wextra -std=c++17 -I include

SRC_DIR  := src
OBJ_DIR  := build_cli
BIN_DIR  := bin

TARGET   := $(BIN_DIR)/nimonspoli_cli

# Sumber CLI: semua .cpp KECUALI src/views/* (kecuali CLIGUI.cpp) dan entry main duplikat
ALL_SRCS := $(shell find $(SRC_DIR) -name '*.cpp')
EXCLUDE  := $(SRC_DIR)/views/GUI.cpp \
            $(SRC_DIR)/views/GUITest.cpp \
            $(SRC_DIR)/views/ThrowDice.cpp \
            $(SRC_DIR)/views/ViewTesting.cpp \
            $(shell find $(SRC_DIR)/views/viewElement $(SRC_DIR)/views/animation -name '*.cpp' 2>/dev/null)

SRCS := $(filter-out $(EXCLUDE), $(ALL_SRCS))
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@
	@echo "CLI build OK -> $(TARGET)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: all
	./$(TARGET)

clean:
	rm -rf $(OBJ_DIR)
	rm -f $(TARGET)
	rm -rf $(BIN_DIR) 2>/dev/null || true

rebuild: clean all

.PHONY: all clean rebuild run
