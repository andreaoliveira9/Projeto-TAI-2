CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

all: models_generator

models_generator: src/models_generator.cpp
	$(CXX) $(CXXFLAGS) -o src/bin/models_generator.out src/models_generator.cpp

clean:
	rm -f models_generator.out