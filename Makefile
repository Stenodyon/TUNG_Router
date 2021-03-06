NAME=router

SRC_DIR=src/
BUILD_DIR=build/
TESTS_DIR=tests/

FLAGS=-std=c++17 -Wall -Wextra -Wpedantic -Wno-sign-compare -Wno-reorder \
      -fms-extensions -flto -I./lib/include
LIBS=-L./lib/lib/x64/ -lSDL2

# -----------------------------------------------------

BIN=$(NAME).exe
BIN_DEBUG=$(NAME).debug.exe
BIN_TEST=$(NAME).test.exe

SRC=$(shell find $(SRC_DIR) -type f -name '*.cpp')
OBJ=$(SRC:$(SRC_DIR)%.cpp=$(BUILD_DIR)%.o)

TEST_SRC=$(shell find $(TESTS_DIR) -type f -name '*.cpp')
TEST_OBJ=$(TEST_SRC:$(TESTS_DIR)%.cpp=$(BUILD_DIR)%.o)

default: release

release: FLAGS+= -O2
release: $(BIN)

debug: FLAGS+= -g -D _DEBUG
debug: $(BIN_DEBUG)

test: FLAGS+= -D _TESTING -I./lib/catch/
test: $(BIN_TEST)
	$(BIN_TEST)

clean:
	-rm -rf $(BUILD_DIR)
	-rm $(BIN) $(BIN_DEBUG) $(BIN_TEST)

.PHONY: release debug test clean

$(BIN): $(OBJ)
	g++ $(FLAGS) $(OBJ) $(LIBS) -o $(BIN)

$(BIN_DEBUG): $(OBJ)
	g++ $(FLAGS) $(OBJ) $(LIBS) -o $(BIN_DEBUG)

$(BIN_TEST): $(OBJ) $(TEST_OBJ)
	g++ $(FLAGS) $(OBJ) $(TEST_OBJ) $(LIBS) -o $(BIN_TEST)

$(BUILD_DIR)%.o: $(SRC_DIR)%.cpp
	@mkdir -p $(dir $@)
	g++ -c $< $(FLAGS) -o $@

$(BUILD_DIR)%.o: $(TESTS_DIR)%.cpp
	@mkdir -p $(dir $@)
	g++ -c $< $(FLAGS) -I./src -o $@
