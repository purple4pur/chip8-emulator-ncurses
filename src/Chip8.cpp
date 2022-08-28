#include "Chip8.h"

#include <chrono>
#include <cstdint>
#include <cstring> // memset()
#include <fstream>
#include <random>
#include <string>

#define V0 0x0
#define VF 0xF // special registor to store instruction result flag


Chip8::Chip8() {
    // Initialize OPTable
    Init_OPTable();

    // Initialize RNG
    rand_gen.seed( std::chrono::system_clock::now().time_since_epoch().count() );
    rand_byte = std::uniform_int_distribution<uint8_t>(0, 0xFF);


    // Initialize pc
    pc = START_ADDRESS;

    // Load fonts into memory
    for (uint16_t i = 0; i < FONTSET_SIZE; i ++) {
        memory[FONTSET_START_ADDRESS + i] = fontset[i];
    }

}

void Chip8::LoadROM(const std::string filename, bool &success) {
    // read file as binary and move to the end
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if (file.is_open()) {
        // get size of the file
        std::streampos size = file.tellg();

        // read ROM to buffer
        char* buffer = new char[size];
        file.seekg(0, std::ios::beg); // move back to beginning
        file.read(buffer, size);
        file.close();

        // load ROM to memory, starting at 0x200
        for (unsigned long i = 0; i < size; i ++) {
            memory[START_ADDRESS + i] = buffer[i];
        }

        delete[] buffer;
    } else {
        success = false;
    }
}

// Fetch ==> Decode ==> Execute
void Chip8::Cycle(bool &success) {
    // invalid pc
    if (pc < 0 || pc + 1 >= 4096 || pc % 2 == 1) {
        success = false;
        return;
    }

    // Fetch
    // concat [pc] and [pc+1] ==> 16-bit opcode
    opcode = (memory[pc] << 8) | memory[pc + 1];

    // move pc to next instruction before doing anything
    pc += 2;

    // Decode & Execute
    ( this->*(OPTable[(opcode & 0xF000) >> 12]) )();

    // run timer if set
    if (delay_timer > 0) delay_timer --;
    if (sound_timer > 0) sound_timer --;
}

// prepare OPTable
void Chip8::Init_OPTable() {
    OPTable[0x0] = &Chip8::to_OPTable_0;
    OPTable[0x1] = &Chip8::OP_1nnn;
    OPTable[0x2] = &Chip8::OP_2nnn;
    OPTable[0x3] = &Chip8::OP_3xkk;
    OPTable[0x4] = &Chip8::OP_4xkk;
    OPTable[0x5] = &Chip8::OP_5xy0;
    OPTable[0x6] = &Chip8::OP_6xkk;
    OPTable[0x7] = &Chip8::OP_7xkk;
    OPTable[0x8] = &Chip8::to_OPTable_8;
    OPTable[0x9] = &Chip8::OP_9xy0;
    OPTable[0xA] = &Chip8::OP_Annn;
    OPTable[0xB] = &Chip8::OP_Bnnn;
    OPTable[0xC] = &Chip8::OP_Cxkk;
    OPTable[0xD] = &Chip8::OP_Dxyn;
    OPTable[0xE] = &Chip8::to_OPTable_E;
    OPTable[0xF] = &Chip8::to_OPTable_F;

    for (int i = 0; i <= 0xE; i ++) OPTable_0[i] = &Chip8::OP_NULL;
    OPTable_0[0x0] = &Chip8::OP_00E0;
    OPTable_0[0xE] = &Chip8::OP_00EE;

    for (int i = 0; i <= 0xE; i ++) OPTable_8[i] = &Chip8::OP_NULL;
    OPTable_8[0x0] = &Chip8::OP_8xy0;
    OPTable_8[0x1] = &Chip8::OP_8xy1;
    OPTable_8[0x2] = &Chip8::OP_8xy2;
    OPTable_8[0x3] = &Chip8::OP_8xy3;
    OPTable_8[0x4] = &Chip8::OP_8xy4;
    OPTable_8[0x5] = &Chip8::OP_8xy5;
    OPTable_8[0x6] = &Chip8::OP_8xy6;
    OPTable_8[0x7] = &Chip8::OP_8xy7;
    OPTable_8[0xE] = &Chip8::OP_8xyE;

    for (int i = 0; i <= 0xA1; i ++) OPTable_E[i] = &Chip8::OP_NULL;
    OPTable_E[0x9E] = &Chip8::OP_Ex9E;
    OPTable_E[0xA1] = &Chip8::OP_ExA1;

    for (int i = 0; i <= 0x65; i ++) OPTable_F[i] = &Chip8::OP_NULL;
    OPTable_F[0x07] = &Chip8::OP_Fx07;
    OPTable_F[0x0A] = &Chip8::OP_Fx0A;
    OPTable_F[0x15] = &Chip8::OP_Fx15;
    OPTable_F[0x18] = &Chip8::OP_Fx18;
    OPTable_F[0x1E] = &Chip8::OP_Fx1E;
    OPTable_F[0x29] = &Chip8::OP_Fx29;
    OPTable_F[0x33] = &Chip8::OP_Fx33;
    OPTable_F[0x55] = &Chip8::OP_Fx55;
    OPTable_F[0x65] = &Chip8::OP_Fx65;
}

// sub-OPTable
void Chip8::to_OPTable_0() {
    (this->*OPTable_0[opcode & 0x000F])();
}
void Chip8::to_OPTable_8() {
    (this->*OPTable_8[opcode & 0x000F])();
}
void Chip8::to_OPTable_E() {
    (this->*OPTable_E[opcode & 0x00FF])();
}
void Chip8::to_OPTable_F() {
    (this->*OPTable_F[opcode & 0x00FF])();
}
// NULL opcode
void Chip8::OP_NULL() {}

// ******  opcode implement  ******
// clear the display
void Chip8::OP_00E0() {
    memset(video, 0, sizeof(video));
}

// return from a subroutine
void Chip8::OP_00EE() {
    sp --;
    pc = stack[sp];
}

// jump to location nnn
void Chip8::OP_1nnn() {
    uint16_t address = opcode & 0x0FFF;
    pc = address;
}

// call subroutine at nnn
void Chip8::OP_2nnn() {
    uint16_t address = opcode & 0x0FFF;
    stack[sp] = pc;
    sp ++;
    pc = address;
}

// skip next instruction if Vx == kk
void Chip8::OP_3xkk() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t byte = opcode & 0x00FF;
    if (registers[Vx] == byte) {
        pc += 2;
    }
}

// skip next instruction if Vx != kk
void Chip8::OP_4xkk() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t byte = opcode & 0x00FF;
    if (registers[Vx] != byte) {
        pc += 2;
    }
}

// skip next instruction if Vx == Vy
void Chip8::OP_5xy0() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t Vy = (opcode & 0x00F0) >> 4;
    if (registers[Vx] == registers[Vy]) {
        pc += 2;
    }
}

// set Vx = kk
void Chip8::OP_6xkk() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t byte = opcode & 0x00FF;
    registers[Vx] = byte;
}

// set Vx = Vx + kk
void Chip8::OP_7xkk() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t byte = opcode & 0x00FF;
    registers[Vx] += byte;
}

// set Vx = Vy
void Chip8::OP_8xy0() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t Vy = (opcode & 0x00F0) >> 4;
    registers[Vx] = registers[Vy];
}

// set Vx = Vx OR Vy
void Chip8::OP_8xy1() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t Vy = (opcode & 0x00F0) >> 4;
    registers[Vx] |= registers[Vy];
}

// set Vx = Vx AND Vy
void Chip8::OP_8xy2() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t Vy = (opcode & 0x00F0) >> 4;
    registers[Vx] &= registers[Vy];
}

// set Vx = Vx XOR Vy
void Chip8::OP_8xy3() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t Vy = (opcode & 0x00F0) >> 4;
    registers[Vx] ^= registers[Vy];
}

// set Vx = Vx + Vy, set VF = carry
void Chip8::OP_8xy4() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t Vy = (opcode & 0x00F0) >> 4;
    uint16_t sum = registers[Vx] + registers[Vy];
    if (sum > 0xFF)
        registers[VF] = 1;
    else
        registers[VF] = 0;
    registers[Vx] = sum & 0xFF;
}

// set Vx = Vx - Vy, set VF = NOT borrow
// Vx > Vy ==> VF = 1
void Chip8::OP_8xy5() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t Vy = (opcode & 0x00F0) >> 4;
    if (registers[Vx] > registers[Vy])
        registers[VF] = 1;
    else
        registers[VF] = 0;
    registers[Vx] -= registers[Vy];
}

// set Vx = Vx >> 1, set VF = the shifted out bit
void Chip8::OP_8xy6() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    registers[VF] = registers[Vx] & 0x0001;
    registers[Vx] >>= 1;
}

// set Vx = Vy - Vx, set VF = NOT borrow
// Vy > Vx ==> VF = 1
void Chip8::OP_8xy7() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t Vy = (opcode & 0x00F0) >> 4;
    if (registers[Vy] > registers[Vx])
        registers[VF] = 1;
    else
        registers[VF] = 0;
    registers[Vx] = registers[Vy] - registers[Vx];
}

// set Vx = Vx << 1, set VF = the shifted out bit
void Chip8::OP_8xyE() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    registers[VF] = registers[Vx] & 0x8000;
    registers[Vx] <<= 1;
}

// skip next instruction if Vx != Vy
void Chip8::OP_9xy0() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t Vy = (opcode & 0x00F0) >> 4;
    if (registers[Vx] != registers[Vy]) {
        pc += 2;
    }
}

// set index (of memory) = nnn
void Chip8::OP_Annn() {
    uint16_t address = opcode & 0x0FFF;
    index = address;
}

// jump to location V0 + nnn
void Chip8::OP_Bnnn() {
    uint16_t address = opcode & 0x0FFF;
    pc = registers[V0] + address;
}

// set Vx = random byte AND kk
void Chip8::OP_Cxkk() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t byte = opcode & 0x00FF;
    registers[Vx] = rand_byte(rand_gen) & byte;
}

// display n-byte sprite starting at index at (Vx, Vy), set VF = collision
// "sprite pixel" = pixel to be plotted
// +--------------> x    i.e. "F" in memory:
// |  (Vx,Vy)               index --> 0xF0
// |    +---------+                   0x80
// |    |    8    |                   0xF0
// |    |n        |                   0x80
// |    |         |                   0x80
// v y  +---------+
void Chip8::OP_Dxyn() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t Vy = (opcode & 0x00F0) >> 4;
    uint8_t height = opcode & 0x000F;

    // wrap if out of display
    uint8_t x_pox = registers[Vx] % VIDEO_WIDTH;
    uint8_t y_pox = registers[Vy] % VIDEO_HEIGHT;

    registers[VF] = 0;
    for (uint8_t row = 0; row < height; row ++) {
        uint8_t sprite_byte = memory[index + row];

        for (uint8_t col = 0; col < 8; col ++) {
            // check if the pixel is within bounds
            if (y_pox + row < VIDEO_HEIGHT && x_pox + col < VIDEO_WIDTH) {
                uint8_t sprite_pixel = sprite_byte & (0x80 >> col);       // get current pixel to be plotted
                uint32_t *screen_pixel = &video[y_pox + row][x_pox + col]; // get current pixel on screen

                // sprite_pixel is ON
                if (sprite_pixel) {
                    // screen_pixel is ON
                    if (*screen_pixel == 0xFFFFFFFF) {
                        registers[VF] = 1; // set collision flag
                    }
                    *screen_pixel ^= 0xFFFFFFFF; // reverse screen_pixel
                }
            }
        }
    }
}

// skip next instruction if key [Vx] is pressed
void Chip8::OP_Ex9E() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t key = registers[Vx];
    if (keypad[key]) {
        pc += 2;
    }
}

// skip next instruction if key [Vx] is NOT pressed
void Chip8::OP_ExA1() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t key = registers[Vx];
    if (!keypad[key]) {
        pc += 2;
    }
}

// set Vx = delay_timer value
void Chip8::OP_Fx07() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    registers[Vx] = delay_timer;
}

// wait for a key press, store the key value in Vx
void Chip8::OP_Fx0A() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t key_num = 0;
    for (; key_num < 0xF; key_num ++) {
        if (keypad[key_num]) {
            registers[Vx] = key_num;
            break;
        }
    }
    if (key_num == 0xF) pc -= 2; // repeat this instruction
}

// set delay_timer = Vx
void Chip8::OP_Fx15() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    delay_timer = registers[Vx];
}

// set sound_timer = Vx
void Chip8::OP_Fx18() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    sound_timer = registers[Vx];
}

// set index = index + Vx
void Chip8::OP_Fx1E() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    index += registers[Vx];
}

// set index = location of the sprite of digit Vx
// i.e. Vx = 5, index --> "5"
void Chip8::OP_Fx29() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t digit = registers[Vx];
    index = FONTSET_START_ADDRESS + (5 * digit);
}

// store BCD of Vx in {index, index + 1, index + 2}
// ==> {Hundreds, Tens, Ones}
void Chip8::OP_Fx33() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t value = registers[Vx];
    memory[index + 2] = value % 10;
    value /= 10;
    memory[index + 1] = value % 10;
    value /= 10;
    memory[index] = value % 10;
}

// store V0-Vx starting at [index]
void Chip8::OP_Fx55() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    for (uint8_t i = 0; i <= Vx; i ++) {
        memory[index + i] = registers[i];
    }
}

// set V0-Vx = [index]...[index + x]
void Chip8::OP_Fx65() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    for (uint8_t i = 0; i <= Vx; i ++) {
        registers[i] = memory[index + i];
    }
}
// ******  end of: opcode implement  ******
