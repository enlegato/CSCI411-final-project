# Makefile for compiling and running the imageTo2DArray program

# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = `pkg-config --cflags opencv4`

# Linker flags
LDFLAGS = `pkg-config --libs opencv4`

# Target executable
TARGET = hcd

# Source files
SRC = hcd.cpp

# Build the target executable
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

# Clean the build directory
clean:
	rm -f $(TARGET)
