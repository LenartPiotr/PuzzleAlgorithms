CXX = g++
MKDIR_P = @mkdir
CXXFLAGS = -std=c++17 -Wall -Iinclude

SRC_DIRS := $(wildcard src/*)
OBJ_DIRS := $(patsubst src/%, obj/%, $(SRC_DIRS))
INCLUDE := $(wildcard include/*.h)
OBJS := $(patsubst src/%.cpp, obj/%.o, $(wildcard src/*/*.cpp))

TARGET = main

all: directories $(TARGET)

directories: $(OBJ_DIRS)

$(OBJ_DIRS):
	$(MKDIR_P) $(subst /,\,$@)

$(TARGET): main.o $(OBJS) $(INCLUDE)
	$(CXX) $(CXXFLAGS) -o $@ $^

obj/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -rf $(TARGET) obj

.PHONY: all clean
