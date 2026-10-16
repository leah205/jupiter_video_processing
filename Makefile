CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -g -pg -Iinclude $(shell pkg-config --cflags opencv4) -Iwavelib/header 
CXXFLAGS += -MMD -MP
-include $(wildcard *.d)
LDFLAGS = -Lwavelib/Bin
LIBS = $(shell pkg-config --libs opencv4) 
LDLIBS = -lwavelib -lwauxlib

SRC_DIR := src
BUILD_DIR := build
BIN_DIR := bin

SOURCES := $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))
DEPS := $(OBJECTS:.o=.d)
TARGET := $(BIN_DIR)/jupiter


.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDLIBS) $(LDFLAGS) $(LIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR) $(BIN_DIR):
	mkdir -p $@

-include $(DEPS)

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)


