CXX    := clang++
SRCDIR := src
INCDIR := include
BUILD  := build
BIN	   := bin
SRCS   := $(addprefix $(SRCDIR)/, main.cpp Camera.cpp Map.cpp EarthGrid.cpp Crosshair.cpp Drawable.cpp Tile.cpp Utils.cpp)
HDRS   := $(addprefix $(INCDIR)/,          Camera.hpp Map.hpp EarthGrid.hpp Crosshair.hpp Drawable.hpp Tile.hpp Utils.hpp)
OBJS   := $(SRCS:$(SRCDIR)/%.cpp=$(BUILD)/%.o)
LIBS   := -lepoxy -lGL -lglfw
# OFLAGS := -Ofast -march=native -flto -std=c++17 -Wall -Wextra -Wpedantic -Werror
TARGET := $(BIN)/main

default: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(BIN)
	@$(CXX) -o $@ $^ $(LIBS) $(OFLAGS)

$(BUILD)/%.o: $(SRCDIR)/%.cpp $(INCDIR)/%.hpp
	@mkdir -p $(BUILD)
	@$(CXX) -c $< -o $@ -I$(INCDIR) $(OFLAGS)

$(BUILD)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(BUILD)
	@$(CXX) -c $< -o $@ -I$(INCDIR) $(OFLAGS)

clean:
	@rm -rf $(BUILD) $(BIN)
