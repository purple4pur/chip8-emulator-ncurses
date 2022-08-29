#include "Platform.h"

#include <chrono>
#include <cstdint>
#include <cstring> // strlen()
#include <filesystem>
#include <ncurses.h>
#include <string>

#define KEY_ESC 27


Platform::Platform(const int min_width, const int min_height, bool &success) {
    success = true;
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    timeout(TIMEOUT); // non-blocking getch()
    row_start = (LINES - min_height) / 2;
    col_start = (COLS  - min_width ) / 2;
    if (LINES < min_height || COLS < min_width) {
        timeout(-1);
        mvprintw(0, 0, "Current window size: H%d * W%d", LINES, COLS);
        mvprintw(1, 0, "Needed  window size: H%d * W%d (Min)", min_height, min_width);
        mvprintw(2, 0, "Press any key to exit");
        refresh();
        getch();
        success = false;
    }
}

Platform::~Platform() {
    endwin();
    printf("Program finished. Exiting...\n");
}

std::string Platform::SelectROM(const char* base_dir, bool &success) {
    timeout(-1);
    mvprintw(row_start + 2, col_start + 2, "Selete a ROM to run: (UP/DOWN/ENTER)");

    // print and store the file list
    int cnt = 0;
    std::string file_list[512] = {};
    for (const auto & file : std::filesystem::directory_iterator(base_dir)) {
        mvprintw(row_start + 4 + cnt, col_start + 8,
                "%s", file.path().string().c_str() + strlen(base_dir) + 1);
        file_list[cnt] = file.path().string();
        cnt ++;
    }

    // handle input and print selector
    int sel = 0;
    int ch = ERR;
    do {
        if (ch == KEY_DOWN) {
            if (sel < cnt - 1)
                sel ++;
            else
                sel = 0;
        } else if (ch == KEY_UP) {
            if (sel > 0)
                sel --;
            else
                sel = cnt - 1;
        } else if (ch == '\n') {
            break;
        } else if (ch == KEY_ESC) {
            success = false;
            return std::string();
        }
        for (int i = 0; i < cnt; i ++) {
            if (i == sel) {
                mvprintw(row_start + 4 + i, col_start + 4, "==>");
            } else {
                mvprintw(row_start + 4 + i, col_start + 4, "   ");
            }
        }
        move(LINES - 1, 0);
        refresh();
    } while(ch = getch());

    erase();
    timeout(TIMEOUT);

    // return selected path
    return file_list[sel];
}

void Platform::UpdateScreen(const uint32_t (&video)[VIDEO_HEIGHT][VIDEO_WIDTH]) {
    for (long y = 0; y < VIDEO_HEIGHT; y ++) {
        for (long x = 0; x < VIDEO_WIDTH; x ++) {
            mvprintw(y + row_start, x + col_start,
                    "%c", ((video[y][x] == 0) ? ' ' : '#') );
        }
    }
    move(LINES - 1, 0);
    refresh();
}

void Platform::DebugInfo(const int cycle_delay, const Chip8 &chip8) {
    mvprintw(row_start, col_start + VIDEO_WIDTH + 2, "[DebugInfo]");

    mvprintw(row_start + 2, col_start + VIDEO_WIDTH + 2, "cycle_delay: %d", cycle_delay);

    mvprintw(row_start + 4, col_start + VIDEO_WIDTH + 2, "opcode: %04X", chip8.opcode);

    int pad_index;
    mvprintw(row_start + 6, col_start + VIDEO_WIDTH + 2, "keypad:");
    for (int i = 0; i < 4; i ++) {
        for (int j = 0; j < 4; j ++) {
            pad_index = i * 4 + j;
            mvprintw(row_start + 7 + i, col_start + VIDEO_WIDTH + 2 + j * 2,
                    "%c", (chip8.keypad[pad_index] == 0) ? '-' :
                          (pad_index <= 9) ? (pad_index + 48) : (pad_index + 55));
        }
    }
    move(LINES - 1, 0);
    refresh();
}

void Platform::ErrorMessage(const char* message) {
    timeout(-1);
    erase();
    mvprintw(row_start, col_start, message);
    getch();
}

void Platform::ErrorMessage(const std::string message) {
    ErrorMessage(message.c_str());
}

bool Platform::CatchInput(uint8_t (&keypad)[16]) {
    int key = getch();

    if (key != KEY_ESC) {
        switch (key) {
            case 'x':
            case 'X':
                keypad[last_key] = 0;
                keypad[0x0] = 1;
                last_key = 0x0;
                last_keypress_time = std::chrono::high_resolution_clock::now();
                break;
            case '1':
                keypad[last_key] = 0;
                keypad[0x1] = 1;
                last_key = 0x1;
                last_keypress_time = std::chrono::high_resolution_clock::now();
                break;
            case '2':
                keypad[last_key] = 0;
                keypad[0x2] = 1;
                last_key = 0x2;
                last_keypress_time = std::chrono::high_resolution_clock::now();
                break;
            case '3':
                keypad[last_key] = 0;
                keypad[0x3] = 1;
                last_key = 0x3;
                last_keypress_time = std::chrono::high_resolution_clock::now();
                break;
            case 'q':
            case 'Q':
                keypad[last_key] = 0;
                keypad[0x4] = 1;
                last_key = 0x4;
                last_keypress_time = std::chrono::high_resolution_clock::now();
                break;
            case 'w':
            case 'W':
                keypad[last_key] = 0;
                keypad[0x5] = 1;
                last_key = 0x5;
                last_keypress_time = std::chrono::high_resolution_clock::now();
                break;
            case 'e':
            case 'E':
                keypad[last_key] = 0;
                keypad[0x6] = 1;
                last_key = 0x6;
                last_keypress_time = std::chrono::high_resolution_clock::now();
                break;
            case 'a':
            case 'A':
                keypad[last_key] = 0;
                keypad[0x7] = 1;
                last_key = 0x7;
                last_keypress_time = std::chrono::high_resolution_clock::now();
                break;
            case 's':
            case 'S':
                keypad[last_key] = 0;
                keypad[0x8] = 1;
                last_key = 0x8;
                last_keypress_time = std::chrono::high_resolution_clock::now();
                break;
            case 'd':
            case 'D':
                keypad[last_key] = 0;
                keypad[0x9] = 1;
                last_key = 0x9;
                last_keypress_time = std::chrono::high_resolution_clock::now();
                break;
            case 'z':
            case 'Z':
                keypad[last_key] = 0;
                keypad[0xA] = 1;
                last_key = 0xA;
                last_keypress_time = std::chrono::high_resolution_clock::now();
                break;
            case 'c':
            case 'C':
                keypad[last_key] = 0;
                keypad[0xB] = 1;
                last_key = 0xB;
                last_keypress_time = std::chrono::high_resolution_clock::now();
                break;
            case '4':
                keypad[last_key] = 0;
                keypad[0xC] = 1;
                last_key = 0xC;
                last_keypress_time = std::chrono::high_resolution_clock::now();
                break;
            case 'r':
            case 'R':
                keypad[last_key] = 0;
                keypad[0xD] = 1;
                last_key = 0xD;
                last_keypress_time = std::chrono::high_resolution_clock::now();
                break;
            case 'f':
            case 'F':
                keypad[last_key] = 0;
                keypad[0xE] = 1;
                last_key = 0xE;
                last_keypress_time = std::chrono::high_resolution_clock::now();
                break;
            case 'v':
            case 'V':
                keypad[last_key] = 0;
                keypad[0xF] = 1;
                last_key = 0xF;
                last_keypress_time = std::chrono::high_resolution_clock::now();
                break;

            // other key or no key
            default:
                // hold keypad status for KEYPRESS_DURATION
                auto current_time = std::chrono::high_resolution_clock::now();
                double dt = std::chrono::duration<double, std::chrono::milliseconds::period>(current_time - last_keypress_time).count();
                if (dt > KEYPRESS_DURATION) {
                    keypad[last_key] = 0;
                }
                break;
        }

    // catch ESC
    } else {
        return false;
    }
    return true;
}
