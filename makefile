CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++17 -I include

SRC_DIR := src
OBJ_DIR := build_cli
BIN_DIR := bin
TARGET := $(BIN_DIR)/nimonspoli_cli

ALL_SRCS := $(shell find $(SRC_DIR) -name '*.cpp' | sort)
GUI_SRCS := \
	$(SRC_DIR)/core/main.cpp \
	$(SRC_DIR)/views/GUI.cpp \
	$(SRC_DIR)/views/GUITest.cpp \
	$(SRC_DIR)/views/ThrowDice.cpp \
	$(SRC_DIR)/views/ViewTesting.cpp \
	$(shell find $(SRC_DIR)/views/viewElement -name '*.cpp' 2>/dev/null) \
	$(shell find $(SRC_DIR)/views/animation -name '*.cpp' 2>/dev/null)

SRCS := $(filter-out $(GUI_SRCS),$(ALL_SRCS))
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

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
	rm -rf $(OBJ_DIR) $(TARGET)

rebuild: clean all

.PHONY: all clean rebuild run
