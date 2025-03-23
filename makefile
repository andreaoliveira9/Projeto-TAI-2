CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

SRC_DIR = src
BIN_DIR = $(SRC_DIR)/bin

all: models_generator metaclass

$(BIN_DIR)/models_generator.out: $(SRC_DIR)/models_generator.cpp
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $(BIN_DIR)/models_generator.out $(SRC_DIR)/models_generator.cpp

$(BIN_DIR)/metaclass.out: $(SRC_DIR)/main.cpp $(SRC_DIR)/metaclass.cpp
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $(BIN_DIR)/metaclass.out $(SRC_DIR)/main.cpp $(SRC_DIR)/metaclass.cpp

models_generator: $(BIN_DIR)/models_generator.out

metaclass: $(BIN_DIR)/metaclass.out

clean:
	rm -f $(BIN_DIR)/models_generator.out $(BIN_DIR)/metaclass.out

.PHONY: all models_generator metaclass clean