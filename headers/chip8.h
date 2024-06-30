#include "bytes.h"

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cstring> // gives memset

// used for creating random seed, used by opcode CXNN
#include <cstdlib> 
#include <ctime>

using namespace bytes;

// enum to index into registers array
enum Register {V0=0, V1, V2, V3, V4, V5, V6, V7, V8, V9, VA, VB, VC, VD, VE, VF /*carry flag*/};

class chip8 {
    private:
    /* Memory
    Memory Map from http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    +---------------+= 0xFFF (4095) End of Chip-8 RAM
    |               |
    |               |
    |               |
    |               |
    |               |
    | 0x200 to 0xFFF|
    |     Chip-8    |
    | Program / Data|
    |     Space     |
    |               |
    |               |
    |               |
    |               |
    |               |
    |               |
    |               |
    +---------------+= 0x200 (512) Start of most Chip-8 programs
    | 0x000 to 0x1FF|
    | Reserved for  |
    |  interpreter  |
    +---------------+= 0x000 (0) Start of Chip-8 RAM
    */
    BYTE memory [4096];  // = 0xFFF 

    // Registers
    BYTE registers [16];
    WORD I; // address register

    // Stack
    std::vector<WORD> stack;
    WORD stack_ptr; // don't use b/c vector
    WORD pc; // program counter
    
    const BYTE font[80] = // Sprites
    { 
        // 0
        0xF0, //    ****    11110000
        0x90, //    *  *    10010000
        0x90, //    *  *    10010000
        0x90, //    *  *    10010000
        0xF0, //    ****    11110000

        // 1
        0x20, //      *     00100000
        0x60, //     **     01100000
        0x20, //      *     00100000
        0x20, //      *     00100000
        0x70, //     ***    01110000

        // 2
        0xF0, //    ****    11110000
        0x10, //       *    00010000
        0xF0, //    ****    11110000
        0x80, //    *       10000000
        0xF0, //    ****    11110000

        // 3
        0xF0, //    ****    11110000
        0x10, //       *    00010000
        0xF0, //    ****    11110000
        0x10, //       *    00010000
        0xF0, //    ****    11110000

        // 4
        0x90, //    *  *    10010000
        0x90, //    *  *    10010000
        0xF0, //    ****    11110000
        0x10, //       *    00010000
        0x10, //       *    00010000

        // 5
        0xF0, //    ****    11110000
        0x80, //    *       10000000
        0xF0, //    ****    11110000
        0x10, //       *    00010000
        0xF0, //    ****    11110000

        // 6
        0xF0, //    ****    11110000
        0x80, //    *       10000000
        0xF0, //    ****    11110000
        0x90, //    *  *    10010000
        0xF0, //    ****    11110000

        // 7
        0xF0, //    ****    11110000
        0x10, //       *    00010000
        0x20, //      *     00100000
        0x40, //     *      01000000
        0x40, //     *      01000000

        // 8
        0xF0, //    ****    11110000
        0x90, //    *  *    10010000
        0xF0, //    ****    11110000
        0x90, //    *  *    10010000
        0xF0, //    ****    11110000

        // 9
        0xF0, //    ****    11110000
        0x90, //    *  *    10010000
        0xF0, //    ****    11110000
        0x10, //       *    00010000
        0xF0, //    ****    11110000

        // A
        0xF0, //    ****    11110000
        0x90, //    *  *    10010000
        0xF0, //    ****    11110000
        0x90, //    *  *    10010000
        0x90, //    *  *    10010000

        // B
        0xE0, //    ***     11100000
        0x90, //    *  *    10010000
        0xE0, //    ***     11100000
        0x90, //    *  *    10010000
        0xE0, //    ***     11100000

        // C
        0xF0, //    ****    11110000
        0x80, //    *       10000000
        0x80, //    *       10000000
        0x80, //    *       10000000
        0xF0, //    ****    11110000

        // D
        0xE0, //    ***     11100000
        0x90, //    *  *    10010000
        0x90, //    *  *    10010000
        0x90, //    *  *    10010000
        0xE0, //    ***     11100000

        // E
        0xF0, //    ****    11110000
        0x80, //    *       10000000
        0xF0, //    ****    11110000
        0x80, //    *       10000000
        0xF0, //    ****    11110000

        // F
        0xF0, //    ****    11110000      
        0x80, //    *       10000000  
        0xF0, //    ****    11110000  
        0x80, //    *       10000000  
        0x80  //    *       10000000  
    };

    // Functions
    WORD fetch(); 
    void decode(WORD opcode);

    // execute - opcode functions
    void opc0NNN(WORD opcode); void opc00E0(); void opc00EE(); void opc1NNN(WORD opcode); void opc2NNN(WORD opcode); 
    void opc3XNN(WORD opcode); void opc4XNN(WORD opcode); void opc5XY0(WORD opcode); void opc6XNN(WORD opcode); void opc7XNN(WORD opcode);
    void opc8XY0(WORD opcode); void opc8XY1(WORD opcode); void opc8XY2(WORD opcode); void opc8XY3(WORD opcode); void opc8XY4(WORD opcode);
    void opc8XY5(WORD opcode); void opc8XY6(WORD opcode); void opc8XY7(WORD opcode); void opc8XYE(WORD opcode); void opc9XY0(WORD opcode);
    void opcANNN(WORD opcode); void opcBNNN(WORD opcode); void opcCXNN(WORD opcode); void opcDXYN(WORD opcode); void opcEX9E(WORD opcode); 
    void opcEXA1(WORD opcode); void opcFX07(WORD opcode); void opcFX0A(WORD opcode); void opcFX15(WORD opcode); void opcFX18(WORD opcode); 
    void opcFX1E(WORD opcode); void opcFX29(WORD opcode); void opcFX33(WORD opcode); void opcFX55(WORD opcode); void opcFX65(WORD opcode);
    
    // Public because emulator needs to access
    public:
    void init(const std::string &game);
    void reset();
    void cycle();
    
    // Timers
    BYTE delay_timer;
    BYTE sound_timer;

    // Input
    BYTE keys [16]; 
    
    // Graphics
    BYTE display [64 * 32]; 
};