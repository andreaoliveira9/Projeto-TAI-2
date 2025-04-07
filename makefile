CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

SRC_DIR = src
BIN_DIR = $(SRC_DIR)/bin

all: models_generator main similarities_levenshtein similarities_models

$(BIN_DIR)/models_generator.out: $(SRC_DIR)/models_generator.cpp
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $(BIN_DIR)/models_generator.out $(SRC_DIR)/models_generator.cpp

$(BIN_DIR)/main.out: $(SRC_DIR)/main.cpp $(SRC_DIR)/MetaClass.cpp
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $(BIN_DIR)/main.out $(SRC_DIR)/main.cpp $(SRC_DIR)/MetaClass.cpp

$(BIN_DIR)/similarities_levenshtein.out: $(SRC_DIR)/similarities_levenshtein.cpp
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $(BIN_DIR)/similarities_levenshtein.out $(SRC_DIR)/similarities_levenshtein.cpp

$(BIN_DIR)/similarities_models.out: $(SRC_DIR)/similarities_models.cpp $(SRC_DIR)/MetaClass.cpp
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $(BIN_DIR)/similarities_models.out $(SRC_DIR)/similarities_models.cpp $(SRC_DIR)/MetaClass.cpp

models_generator: $(BIN_DIR)/models_generator.out

main: $(BIN_DIR)/main.out

similarities_levenshtein: $(BIN_DIR)/similarities_levenshtein.out

similarities_models: $(BIN_DIR)/similarities_models.out

clean:
	rm -f $(BIN_DIR)/models_generator.out $(BIN_DIR)/main.out $(BIN_DIR)/similarities_levenshtein.out $(BIN_DIR)/similarities_models.out

.PHONY: all models_generator main similarities_levenshtein similarities_models clean