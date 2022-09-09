help:
	@echo "Help menu:"
	@echo "    'make build'        build for release"
	@echo "    'make build-debug'  build for debug"
	@echo "    'make build-all'    build both"
	@echo "    'make clean'"

# ******************************************************

CXX         = g++
FLAGS       = -std=c++17 -lncurses
OPTFLAGS    = -O2
DBGFLAGS    = -D DEBUG -g

SRCFILES    = $(shell find ./src -type f -name "*.cpp")

build:
	$(CXX) $(SRCFILES) $(FLAGS) $(OPTFLAGS) \
		-o chip8_emulator

build-debug:
	$(CXX) $(SRCFILES) $(FLAGS) $(DBGFLAGS) \
		-o chip8_emulator_debug

build-all: build build-debug

clean:
	-rm -f chip8_emulator chip8_emulator_debug

.PHONY: help build build-debug build-all clean
