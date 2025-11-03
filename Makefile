CXX = g++
CXXFLAGS = -Iinclude -std=c++17 -Wall -O2

SRC = simulator.cpp src/*.cpp

TARGET = sim

all: $(TARGET)

$(TARGET):
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)
