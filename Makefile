.PHONY: help
help:
	@echo "  Usage:"
	@echo "    make <target>"
	@echo ""
	@echo "  <target>:"
	@echo "    run   [t=3]     (re)build & run chip8_emulator"
	@echo "    debug [t=3]     (re)build & run chip8_emulator_debug"
	@echo "    build           (re)build chip8_emulator and chip8_emulator_debug"
	@echo "    clean"
	@echo ""
	@echo "  [t]:"
	@echo "    Cycle interval in (ms) between [2,5000], default value is 3."

# ******************************************************

CXX         = g++
FLAGS       = -std=c++17 -lncurses
OPTFLAGS    = -O2
DBGFLAGS    = -D DEBUG -g

SRCFILES    = $(shell find ./src -type f -name "*.cpp")
t           = 3

.PHONY: run
run: chip8_emulator
	./$^ $(t)

.PHONY: debug
debug: chip8_emulator_debug
	./$^ $(t)

chip8_emulator: $(SRCFILES) Makefile
	$(CXX) $(SRCFILES) $(FLAGS) $(OPTFLAGS) \
		-o $@

chip8_emulator_debug: $(SRCFILES) Makefile
	$(CXX) $(SRCFILES) $(FLAGS) $(DBGFLAGS) \
		-o $@

.PHONY: build
build: chip8_emulator chip8_emulator_debug

.PHONY: clean
clean:
	rm -rf chip8_emulator chip8_emulator_debug
