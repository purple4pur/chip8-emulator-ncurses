#ifndef __CHIP8_H__
#define __CHIP8_H__

#include <cstdint>
#include <random>
#include <string>

const uint16_t START_ADDRESS         = 0x200;
const uint16_t FONTSET_START_ADDRESS = 0x50;
const uint16_t FONTSET_SIZE          = 80;
const uint8_t  VIDEO_WIDTH           = 64;
const uint8_t  VIDEO_HEIGHT          = 32;


class Chip8 {
  public:
    // Chip initialization
    Chip8();
    // Load ROM from file
    void LoadROM(const std::string filename, bool &success);
    // Fetch ==> Decode ==> Execute
    void Cycle(bool &success);

    uint8_t   registers   [16]       = {};
    uint8_t   memory      [4096]     = {};
    uint16_t  index                  = {}; // index of memory
    uint16_t  pc                     = {}; // program pointer
    uint16_t  stack       [16]       = {};
    uint8_t   sp                     = {}; // stack pointer
    uint8_t   delay_timer            = {};
    uint8_t   sound_timer            = {};
    uint32_t  opcode;
    uint8_t   keypad      [16]       = {};
    uint32_t  video   [VIDEO_HEIGHT]       // all pixels on display
                      [VIDEO_WIDTH ] = {}; // video[y][x], each pixel: full 0/F

  private:
    typedef void (Chip8::*OP)(void);
    OP        OPTable     [0xF  + 1] = {}; // 0x0 ~ 0xF
    OP        OPTable_0   [0xE  + 1] = {}; // 0x0 ~ 0xE
    OP        OPTable_8   [0xE  + 1] = {}; // 0x0 ~ 0xE
    OP        OPTable_E   [0xA1 + 1] = {}; // 0x0 ~ 0xA1
    OP        OPTable_F   [0x65 + 1] = {}; // 0x0 ~ 0x55

    std::default_random_engine             rand_gen;
    std::uniform_int_distribution<uint8_t> rand_byte;

    uint8_t fontset[FONTSET_SIZE] = {
        /* "F" ==>
         * 11110000
         * 10000000
         * 11110000
         * 10000000
         * 10000000
         */
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    // prepare OPTable
    void Init_OPTable();
    // sub-OPTable
    void to_OPTable_0();
    void to_OPTable_8();
    void to_OPTable_E();
    void to_OPTable_F();
    // NULL opcode
    void OP_NULL();

    // ******  opcode implement  ******
    // clear the display
    void OP_00E0();
    // return from a subroutine
    void OP_00EE();
    // jump to location nnn
    void OP_1nnn();
    // call subroutine at nnn
    void OP_2nnn();
    // skip next instruction if Vx == kk
    void OP_3xkk();
    // skip next instruction if Vx != kk
    void OP_4xkk();
    // skip next instruction if Vx == Vy
    void OP_5xy0();
    // set Vx = kk
    void OP_6xkk();
    // set Vx = Vx + kk
    void OP_7xkk();
    // set Vx = Vy
    void OP_8xy0();
    // set Vx = Vx OR Vy
    void OP_8xy1();
    // set Vx = Vx AND Vy
    void OP_8xy2();
    // set Vx = Vx XOR Vy
    void OP_8xy3();
    // set Vx = Vx + Vy, set VF = carry
    void OP_8xy4();
    // set Vx = Vx - Vy, set VF = NOT borrow
    // Vx > Vy ==> VF = 1
    void OP_8xy5();
    // set Vx = Vx >> 1, set VF = the shifted out bit
    void OP_8xy6();
    // set Vx = Vy - Vx, set VF = NOT borrow
    // Vy > Vx ==> VF = 1
    void OP_8xy7();
    // set Vx = Vx << 1, set VF = the shifted out bit
    void OP_8xyE();
    // skip next instruction if Vx != Vy
    void OP_9xy0();
    // set index (of memory) = nnn
    void OP_Annn();
    // jump to location V0 + nnn
    void OP_Bnnn();
    // set Vx = random byte AND kk
    void OP_Cxkk();
    // display n-byte sprite starting at index at (Vx, Vy), set VF = collision
    // "sprite pixel" = pixel to be plotted
	// +--------------> x    i.e. 'F' in memory:
	// |  (Vx,Vy)               index --> 0xF0
	// |    +---------+                   0x80
	// |    |    8    |                   0xF0
	// |    |n        |                   0x80
	// |    |         |                   0x80
	// v y  +---------+
    void OP_Dxyn();
    // skip next instruction if key [Vx] is pressed
    void OP_Ex9E();
    // skip next instruction if key [Vx] is NOT pressed
    void OP_ExA1();
    // set Vx = delay_timer value
    void OP_Fx07();
    // wait for a key press, store the key value in Vx
    void OP_Fx0A();
    // set delay_timer = Vx
    void OP_Fx15();
    // set sound_timer = Vx
    void OP_Fx18();
    // set index = index + Vx
    void OP_Fx1E();
    // set index = location of the sprite of digit Vx
    // i.e. Vx = 5, index --> "5"
    void OP_Fx29();
    // store BCD of Vx in {index, index + 1, index + 2}
    // ==> {Hundreds, Tens, Ones}
    void OP_Fx33();
    // store V0-Vx starting at [index]
    void OP_Fx55();
    // set V0-Vx = [index]...[index + x]
    void OP_Fx65();
    // ******  end of: opcode implement  ******
};

#endif // __CHIP8_H__
