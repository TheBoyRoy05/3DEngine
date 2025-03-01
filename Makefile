CXX = g++
CXXFLAGS = -g -Wall -Werror #-fsanitize=address
LIBS = -lSDL2 -lSDL2main

SRC_DIR = src
OBJ_DIR = objects
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))
TARGET = engine.exe

# Ensure the objects directory exists
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

all: $(TARGET)

# Compile source files into object files inside objects/
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Build the executable in the root directory
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

# Run the executable
run: $(TARGET)
	./$(TARGET)

# Run the executable with valgrind
test: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

# Clean build artifacts
clean:
	rm -rf $(OBJ_DIR) $(TARGET)
