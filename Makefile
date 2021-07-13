CXX := g++
TARGET := cell_move_router
OPENMPFLAG := 
CXXFLAGS := -std=c++11 -O3 -static -static-libstdc++ -static-libgcc
WARNINGFLAGS := -Wall -Wextra
FLUTE_DIR := src/include/Third/Flute3
INCLUDE := src/include
SRC_DIRS := src\
			src/lib\
			src/lib/FluteWrapper\
			src/lib/cell_move_router\
			src/lib/cell_move_router/Input/Raw\
			src/lib/cell_move_router/Input/Processed\
			src/lib/cell_move_router/Grid\
			src/lib/cell_move_router/Router\
			src/lib/cell_move_router/RegionCalculator\
			src/lib/cell_move_router/Mover
SRCS := $(wildcard $(SRC_DIRS:=/*.cpp))
OBJS := $(SRCS:.cpp=.o) $(FLUTE_DIR)/build/flute.o
DEPS = $(OBJS:.o=.d)

ifndef BOOST_LIBRARY_PATH
BOOST_LIBRARY_PATH := ""
endif

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ -lpthread $(OPENMPFLAG)

$(FLUTE_DIR)/build/flute.o: $(FLUTE_DIR)/flute.cpp
	mkdir -p $(FLUTE_DIR)/build
	$(CXX) $(CXXFLAGS) -w -isystem $(FLUTE_DIR) -MMD -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(WARNINGFLAGS) $(OPENMPFLAG) -I $(INCLUDE) -isystem $(BOOST_LIBRARY_PATH) -MMD -c $< -o $@

clean:
	rm -rf $(TARGET) $(OBJS) $(DEPS)

ifeq (test,$(firstword $(MAKECMDGOALS)))
  TEST_CASE_ID := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  $(eval $(TEST_CASE_ID):;@:)
endif

test: $(TARGET)
	@echo test on case$(TEST_CASE_ID)
	./$(TARGET) benchmark/case$(TEST_CASE_ID).txt evaluator/output_$(TEST_CASE_ID).txt
	./evaluator/evaluator benchmark/case$(TEST_CASE_ID).txt evaluator/output_$(TEST_CASE_ID).txt

.PHONY: all clean test
-include $(DEPS)