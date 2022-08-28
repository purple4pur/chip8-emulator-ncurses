#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include "Chip8.h"

#include <chrono>
#include <cstdint>
#include <string>

#define TIMEOUT 0             // timeout for catch keyboard input
#define KEYPRESS_DURATION 100 // timeout for holding a keypress (ms)


class Platform {
  public:
    Platform(const int screen_width, const int screen_height, bool &success);
    ~Platform();

    std::string SelectROM(const char* base_dir, bool &success);

    void UpdateScreen(const uint32_t (&video)[VIDEO_HEIGHT][VIDEO_WIDTH]);

    void DebugInfo(const int cycle_delay, const Chip8 &chip8);

    // return false if an ESC is pressed
    bool CatchInput(uint8_t (&keypad)[16]);

    void ErrorMessage(const char* message);
    void ErrorMessage(const std::string message);

  private:
    long row_start;
    long col_start;
    char last_key = 0;
    std::chrono::high_resolution_clock::time_point last_keypress_time
        = std::chrono::high_resolution_clock::now();
};

#endif // __PLATFORM_H__
