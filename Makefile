NAME=router

SRC_DIR=src/
BUILD_DIR=build/
TESTS_DIR=tests/

FLAGS=-std=c++17 -Wall -Wextra -Wno-sign-compare -Wno-reorder \
      -fms-extensions -flto -I./lib/SDL2/include/ -I./lib/catch_include/ \
      -I./lib/wx/include/ -I./lib/wx/lib/mswu \
      -D __WXMSW__ -D _UNICODE -D NDEBUG
LIBS=-L./lib/SDL2/lib/x64/ -L./lib/wx/lib/ -lSDL2 \
     -lwxmsw30u_core -lwxbase30u -lwxmsw30u_adv

# -----------------------------------------------------

BIN=$(NAME).exe
BIN_DEBUG=$(NAME).debug.exe
BIN_TEST=$(NAME).test.exe

RES=$(shell find $(SRC_DIR) -type f -name '*.rc')
RES_OBJ=$(RES:$(SRC_DIR)%.rc=$(BUILD_DIR)%.o)
SRC=$(shell find $(SRC_DIR) -type f -name '*.cpp')
OBJ=$(SRC:$(SRC_DIR)%.cpp=$(BUILD_DIR)%.o) $(RES_OBJ)

TEST_SRC=$(shell find $(TESTS_DIR) -type f -name '*.cpp')
TEST_OBJ=$(TEST_SRC:$(TESTS_DIR)%.cpp=$(BUILD_DIR)%.o)

FLAGS+= -MT $@ -MMD -MP -MF $(dir $@).d/$(basename $(notdir $@)).d

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
	@mkdir -p $(dir $@).d/
	g++ -c $< $(FLAGS) -o $@

$(BUILD_DIR)%.o: $(TESTS_DIR)%.cpp
	@mkdir -p $(dir $@)
	@mkdir -p $(dir $@).d/
	g++ -c $< $(FLAGS) -I./src -o $@

$(BUILD_DIR)%.o: $(SRC_DIR)%.rc
	@mkdir -p $(dir $@)
	windres --use-temp-file -I./lib/wx/include $< $@

include $(shell find $(DEP_DIR) -type f -name '*.d')
