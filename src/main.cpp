#include "Chip8.h"
#include "Platform.h"

#include <chrono>
#include <ncurses.h>
#include <stdexcept>
#include <string>


int main(int argc, char** argv) {
    bool success = true;
    int cycle_delay = 3;

    if (argc == 2) {
        try {
            cycle_delay = std::stoi(argv[1]);
        } catch (std::invalid_argument const& ex) {
            printf("Invalid delay '%s'.\nExiting...\n", argv[1]);
            return 1;
        }
        if (cycle_delay < 2) cycle_delay = 2;
        if (cycle_delay > 5000) cycle_delay = 5000;
    }

    Platform platform(VIDEO_WIDTH, VIDEO_HEIGHT, success);
    if (!success) return 1;

    std::string rom_filename = platform.SelectROM("rom", success);
    if (!success) return 0; // pressed ESC

    Chip8 chip8;
    chip8.LoadROM(rom_filename, success);
    if (!success) {
        std::string msg = "[ERROR] Failed to open ROM file '" + rom_filename + "'.";
        platform.ErrorMessage(msg);
        return 1;
    }

    if (success) {
        auto last_cycle_time = std::chrono::high_resolution_clock::now();

        while (platform.CatchInput(chip8.keypad)) {
            auto current_time = std::chrono::high_resolution_clock::now();
            double dt = std::chrono::duration<double, std::chrono::milliseconds::period>(current_time - last_cycle_time).count();

            if (dt > cycle_delay) {
                last_cycle_time = current_time;
                chip8.Cycle(success);
                if (!success) {
                    platform.ErrorMessage("[ERROR] Invalid pc value in runtime.");
                    return 1;
                }
                platform.UpdateScreen(chip8.video);
                #ifdef DEBUG
                    platform.DebugInfo(cycle_delay, chip8);
                #endif
            }
        }
    }

    return 0;
}
