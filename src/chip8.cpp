#include "../headers/chip8.h"
#include <cstdio>

void chip8::init(const std::string &game) {
    I = 0; // reset address register
    pc = 0x200; // chip8 programs start here
    stack_ptr = 0; // reset stack pointer

    memset(registers, 0, sizeof(registers)); // clear registers
    memset(display, 0, sizeof(display)); // clear display
    stack.clear(); // clear stack
    memset(memory, 0, sizeof(memory)); // clear memory
    memset(keys, 0, sizeof(keys)); // clear keys
    srand(time(0)); // seed random number generator, used by opcode CXNN

    // reset timers
    delay_timer = 0;
    sound_timer = 0;

    // load game data
    std::cout << "Loading " << game.c_str() << "\n";
    FILE *f = fopen(game.c_str(), "rb");

    if (f == NULL) {
        std::cerr << "Failed to open file: " << game << "\n";
        fclose(f);
        return;
    }
    
    // load font into memory
    for (int i = 0; i < 80; i++)
        memory[i] = font[i];

    // get file size
    fseek(f, 0, SEEK_END);
    long filesize = ftell(f);
    fseek(f, 0, SEEK_SET);
    std::cout << "File has size of " << filesize << " bytes\n";
    if (filesize > (4096 - 0x200)) {
        std::cerr << "File is too large to fit in memory" << "\n";
        fclose(f);
        return;
    }

    // read data from f into memory starting at the address of byte 0x200
    size_t bytesread = fread(&memory[0x200], 1, filesize, f);
    fclose(f);

    if (bytesread != filesize) {
        std::cerr << "Failed to read entire file. Read " << bytesread << " bytes out of " << filesize << " bytes";
        return;
    }

    std::cout << "File read succesfully. " << bytesread << " bytes read.\n";
    std::cout << "First 16 bits: ";
    for (int i = 0x200; i < 0x210; i++)
        std::cout << memory[i] << " ";
}

// Main difference with init is that game and font already loaded into memory
void chip8::reset() {
    I = 0; 
    pc = 0x200; 
    stack_ptr = 0; 

    memset(registers, 0, sizeof(registers));
    memset(display, 0, sizeof(display));
    stack.clear(); 
    memset(keys, 0, sizeof(keys)); 
    srand(time(0)); 

    delay_timer = 0;
    sound_timer = 0;
}

WORD chip8::fetch() {
    // each 16 bit opcode is comprised of the current and next byte in memory
    WORD opcode = memory [pc] << 8 | memory [++pc] ;
    pc++;
    return opcode;
}

void chip8::decode(WORD opcode) {
    // printf("OPCODE: %#x\n", opcode);

    switch (((opcode & 0xF000) >> 12)) {
        case 0x0:
        {
            switch (opcode & 0x00FF) {
                case 0xE0:    opc00E0();        break;
                case 0xEE:    opc00EE();        break;
                default:      opc0NNN(opcode);  break;
            }
        }
        break;
        case 0x1:    opc1NNN(opcode);           break;
        case 0x2:    opc2NNN(opcode);           break;
        case 0x3:    opc3XNN(opcode);           break;
        case 0x4:    opc4XNN(opcode);           break;
        case 0x5:    opc5XY0(opcode);           break;
        case 0x6:    opc6XNN(opcode);           break;
        case 0x7:    opc7XNN(opcode);           break;
        case 0x8: 
        {
            switch(opcode & 0x000F) {
                case 0x0:    opc8XY0(opcode);                                break;
                case 0x1:    opc8XY1(opcode);                                break;
                case 0x2:    opc8XY2(opcode);                                break;
                case 0x3:    opc8XY3(opcode);                                break;
                case 0x4:    opc8XY4(opcode);                                break;
                case 0x5:    opc8XY5(opcode);                                break;
                case 0x6:    opc8XY6(opcode);                                break;
                case 0x7:    opc8XY7(opcode);                                break;
                case 0xE:    opc8XYE(opcode);                                break;
                default:     printf("Error: invalid opcode %#x\n", opcode);  break;
            }
        }
        break;
        case 0x9:   opc9XY0(opcode);        break;
        case 0xA:   opcANNN(opcode);        break;
        case 0xB:   opcBNNN(opcode);        break;
        case 0xC:   opcCXNN(opcode);        break;
        case 0xD:   opcDXYN(opcode);        break;
        case 0xE:
        {
            switch(opcode & 0x00FF) {
                case 0x9E:  opcEX9E(opcode);                                    break;
                case 0xA1:  opcEXA1(opcode);                                    break;
                default:    printf("Error: invalid opcode %#x\n", opcode);      break;
            }
        }
        break;
        case 0xF:
        {
            switch (opcode & 0x0FF) {
                case 0x07:  opcFX07(opcode);                                break;
                case 0x0A:  opcFX0A(opcode);                                break;
                case 0x15:  opcFX15(opcode);                                break;
                case 0x18:  opcFX18(opcode);                                break;
                case 0x1E:  opcFX1E(opcode);                                break;
                case 0x29:  opcFX29(opcode);                                break;
                case 0x33:  opcFX33(opcode);                                break;
                case 0x55:  opcFX55(opcode);                                break;
                case 0x65:  opcFX65(opcode);                                break;
                default:    printf("Error: invalid opcode %#x\n", opcode);  break;
            }
        }
        break;
        default:
        printf("Error: invalid opcode %#x\n", opcode); 
        break;
    }
}

// Jumpy to machine code routine at addres NNN
void chip8::opc0NNN(WORD opcode) {
    // Ignored on modern computers
}

// Clears the screen
void chip8::opc00E0() {
    memset(display, 0, sizeof(display));
}

// Returns from subroutine
void chip8::opc00EE(){
    pc = stack.back();
    stack.pop_back();
} 

// Jumps to address NNN
void chip8::opc1NNN(WORD opcode){
    // Remember current address 
    // Jump to NNN
    pc = (opcode & 0x0FFF);
} 

// Call subroutine at NNN
void chip8::opc2NNN(WORD opcode) {
    stack.push_back(pc);
    pc = (opcode & 0x0FFF);
} 

// Skips next instruction if VX == NN
void chip8::opc3XNN(WORD opcode) {
    int X = (opcode & 0x0F00) >> 8;
    BYTE NN = (opcode & 0x00FF);
    if (registers[X] == NN)
        pc+=2;
} 

// Skips next instruction if VX != NN
void chip8::opc4XNN(WORD opcode) {
    int x = (opcode & 0x0F00) >> 8;
    BYTE NN = (opcode & 0x00FF);
    if (registers[x] != NN)
        pc+=2;
} 

// Skips next instruction if VX == VY
void chip8::opc5XY0(WORD opcode) {
    int x = (opcode & 0x0F00) >> 8;
    int y = (opcode & 0x00F0) >> 4;
    if (registers[x] == registers[y])
        pc+=2;
}

// Sets VX to NN
void chip8::opc6XNN(WORD opcode) {
    int x = (opcode & 0x0F00) >> 8;
    BYTE NN = (opcode & 0x00FF);
    registers[x] = NN;
}

// Adds NN to VX
void chip8::opc7XNN(WORD opcode) {
    int x = (opcode & 0x0F00) >> 8;
    BYTE NN = (opcode & 0x00FF);
    registers[x] += NN;
}

// Set VX to value of VY
void chip8::opc8XY0(WORD opcode) {
    int x = (opcode & 0x0F00) >> 8;
    int y = (opcode & 0x00F0) >> 4;
    registers[x] = registers[y];
}

// Sets VX to VX | VY
void chip8::opc8XY1(WORD opcode) {
    int x = (opcode & 0x0F00) >> 8;
    int y = (opcode & 0x00F0) >> 4;
    registers[x] |= registers[y];
}

// Sets VX to VX & VY
void chip8::opc8XY2(WORD opcode) {
    int x = (opcode & 0x0F00) >> 8;
    int y = (opcode & 0x00F0) >> 4;
    registers[x] &= registers[y];
}

// Sets VX to VX ^ VY
void chip8::opc8XY3(WORD opcode) {
    int x = (opcode & 0x0F00) >> 8;
    int y = (opcode & 0x00F0) >> 4;
    registers[x] ^= registers[y];
}

// Add VY to VX and set VF to 1 if overflow else 0
void chip8::opc8XY4(WORD opcode) {
    int x = (opcode & 0x0F00) >> 8;
    int y = (opcode & 0x00F0) >> 4;

    // Overflows if vx + vy > 0xFF      ==>     vy > 0xFF - vx
    registers[VF] = 0;
    if (registers[y] > (0xFF - registers[x]))
        registers[VF] = 1;

    registers[x] += registers[y];
}

// Subtract VY from VX and set VF to 0 if underflow else 1
void chip8::opc8XY5(WORD opcode) {
    int x = (opcode & 0x0F00) >> 8;
    int y = (opcode & 0x00F0) >> 4;

    // Underflows if vx - vy < 0        ==>     if vx < vy
    registers[VF] = 1;
    if (registers[x] < registers[y])
        registers[VF] = 0;
    
    registers[x] -= registers[y];
}

// Shift VX to right by 1, store LSB of VX before shift into VF
void chip8::opc8XY6(WORD opcode) {
    int x = (opcode & 0x0F00) >> 8;
    BYTE VX = registers[x];
    BYTE LSB = VX & 1;
    registers[x] >>= 1;
    registers[VF] = LSB;
}

// Subtract VX from VY and set VX to the difference and set VF to 0 if underflow else 1
void chip8::opc8XY7(WORD opcode) {
    int x = (opcode & 0x0F00) >> 8;
    int y = (opcode & 0x00F0) >> 4;

    // Underflow if vy - vx < 0     ==>     if vy < vx
    registers[VF] = 1;
    if (registers[y] < registers[x])
        registers[VF] = 0;

    registers[x] = (registers[y] - registers[x]);
}

// Shift VX to left by 1, store MSB of VX before shift into VF
void chip8::opc8XYE(WORD opcode) {
    int x = (opcode & 0x0F00) >> 8;
    BYTE VX = registers[x];
    BYTE MSB = (VX & 128) >> 7;
    registers[x] <<= 1;
    registers[VF] = MSB;
}

// Skips the next instruction if VX != VY
void chip8::opc9XY0(WORD opcode) {
    int x = (opcode & 0x0F00) >> 8;
    int y = (opcode & 0x00F0) >> 4;
    if (registers[x] != registers[y])
        pc+=2;
}

// Sets I to address NNN
void chip8::opcANNN(WORD opcode) {
    WORD NNN = opcode & 0x0FFF;
    I = NNN;
}

// Jumps to address NNN plus V0
void chip8::opcBNNN(WORD opcode) {
    WORD NNN = opcode & 0x0FFF;
    pc = (registers[V0] + NNN);
}

// Set VX to a random num [0, 255] & NN
void chip8::opcCXNN(WORD opcode) {
    int x = (opcode & 0x0F00) >> 8;
    int NN = opcode & 0x00FF;
    int rnd = rand() % 256;
    registers[x] = rnd & NN;
}

// Draw a sprite on coordinate (VX, VY) with width = 8px, height = N pixels (num rows to draw)
// Each row of pixels read starting from memory location I
// VF set to 1 if pixels flipped else 0
void chip8::opcDXYN(WORD opcode) {
    int x = (opcode & 0x0F00) >> 8;
    int y = (opcode & 0x00F0) >> 4;
    int N = (opcode & 0x000F);
    int VX = registers[x]; 
    int VY = registers[y];
    registers[VF] = 0;

    // For each row (going down the screen)
    for (int i = 0; i < N; i++) {
        int ycoord = (VY+i)%32;
        BYTE sprite = memory [I+i];

        // For each col (going across the screen)
        for (int j = 0; j < 8; j++) {
            int xcoord = (VX+j)%64;
            
            // Iterate thru pixel in the row of this sprite
            BYTE this_pixel = 0b10000000 >> j;

            // If this pixel is toggled on
            if (sprite & this_pixel) {
                int coord = ycoord*64+xcoord;

                // Collision detected
                if (display[coord] == 1)
                    registers[VF] = 1;

                display[coord] ^= 1;           
            }
        }
    }
}

// Skip next instruction if key in VX is pressed
void chip8::opcEX9E(WORD opcode) {
    int x = (opcode & 0x0F00) >> 8;
    if (keys[(registers[x])] != 0)
        pc+=2;
}

// Skip next instruction if key in VX is not pressed
void chip8::opcEXA1(WORD opcode) {
    int x = (opcode & 0x0F00) >> 8;
    if (keys[(registers[x])] == 0)
        pc+=2;
}

// Set VX to value of delay timer
void chip8::opcFX07(WORD opcode) {
    int x = (opcode & 0x0F00) >> 8;
    registers[x] = delay_timer;
}

// A key press is awaited, then stored in VX (halt all instructions until next event)
void chip8::opcFX0A(WORD opcode) {
    int x = (opcode & 0x0F00) >> 8;
    bool anypress = false;

    // Iterate thru keys
    for (int i = 0; i < 16; i++) {
        // If a key is pressed, store it 
        if (keys[i] != 0) {
            registers[x] = i;
            anypress = 1;
        }
    }

    // This instruction will keep executing until a key has been found
    if (!anypress)
        pc-=2;
}

// Sets delay timer to VX
void chip8::opcFX15(WORD opcode) {
    int x = (opcode & 0x0F00) >> 8;
    delay_timer = registers[x];
}

// Sets sound timer to VX
void chip8::opcFX18(WORD opcode) {
    int x = (opcode & 0x0F00) >> 8;
    sound_timer = registers[x];
}

// Add VX to I. Don't care about VF
void chip8::opcFX1E(WORD opcode) {
    int x = (opcode & 0x0F00) >> 8;
    I += registers[x];
}

// Set I to location of sprite for character in VX
void chip8::opcFX29(WORD opcode) {
    int x = (opcode & 0x0F00) >> 8;
    
    // Each sprite takes up 5 bytes
    I = (registers[x])*5;
}

// Store binary coded decimal representation of VX with hundreds digit in location I, tens digit at I+1, and ones digit at I+2
void chip8::opcFX33(WORD opcode) {
    int x = (opcode & 0x0F00) >> 8;
    BYTE bcd = registers[x]; 
    memory[I+2] = bcd % 10;
    bcd /= 10;
    memory[I+1] = bcd % 10;
    bcd /= 10;
    memory[I] = bcd;
}

// Stores from V0 to VX in memory, starting at I. 
void chip8::opcFX55(WORD opcode) {
    int x = (opcode & 0x0F00) >> 8;
    for (int i = 0; i <= x; i++) 
        memory[I+i] = registers[i];
    I += (x + 1); // CHIP-8 version determines whether I is incremented. 
}

// Fills from V0 to VX with values from memory, starting at I.
void chip8::opcFX65(WORD opcode) {
    int x = (opcode & 0x0F00) >> 8;
    for (int i = 0; i <= x; i++) 
        registers[i] = memory[I+i];
    I += (x + 1); // CHIP-8 version determines whether I is incremented. 
}

// CPU: Fetch-decode-execute cycle 
void chip8::cycle() {
    WORD opcode = fetch();
    decode(opcode);
}