# CHIP-8-Emulator
A CHIP-8 Emulator capable of loading and playing most CHIP-8 ROMs. Built with C++ utilizing SDL2 for graphics and input.

This project implements the core functionalities of CHIP-8 including CPU emulation, memory management, and graphics rendering. It also handles input processing, with plans to integrate sound. I’m actively debugging and optimizing the emulator to improve compatibility, performance, and accuracy. It also supports pause and reset.

### Select CHIP-8 ROMs running on my emulator
Some of the games I tested. 
| Tetris        | Breakout      |     
| ------------- |:-------------:|
| ![image](https://github.com/nicholaswile/CHIP-8-Emulator/assets/74445404/68f55dbe-40d8-4cde-b695-975c22a17559) | ![image](https://github.com/nicholaswile/CHIP-8-Emulator/assets/74445404/788300e4-f820-4698-9710-9b8abb12a958)|

| Pong          | Space Invaders|     
| ------------- |:-------------:|
| ![image](https://github.com/nicholaswile/CHIP-8-Emulator/assets/74445404/d9c07efa-6b8e-4e30-8ba8-c255470d12aa) | ![image](https://github.com/nicholaswile/CHIP-8-Emulator/assets/74445404/af6c55f6-c0e1-40bb-9c71-f72b3dee42cf) |

### Motivation
Many CHIP-8 emulators have come before mine, and I’m sure many will come after. I began this project to get more experience with low-level programming, computer organization and architecture, system emulation, and bitwise operations, in preparation for developing a more ambitious emulator for a real video game console. 

## Using the emulator
Change to the same directory as the Makefile. Then execute the following command:
```bash
mingw32-make
./main
```

I use SDL2-developer-2.30.4-mingw32. To compile the code on your machine, you'll need to download that version of SDL2, place the include/SDL2 and lib folders in the src folder, and the SDL2 DLL file in the same directory as the Makefile.

### Selecting a game
The emulator reads from the list of ROMs specified in the config file. To add your own CHIP-8 ROMs, copy them into the ROMs folder (or if you want to create your own folder, specify it in the config file). Then run the emulator. You will be prompted to select a ROM. Type the number of the ROM you want to play, and press enter. The emulator will indicate whether the game was found and loaded succesfully. For example:

```bash
Select a ROM to play on CHIP-8!
0 - game-roms/Blinky [Hans Christian Egeberg, 1991].ch8 
1 - game-roms/Breakout [Carmelo Cortez, 1979].ch8       
2 - game-roms/Connect 4 [David Winter].ch8
3 - game-roms/Kaleidoscope [Joseph Weisbecker, 1978].ch8
4 - game-roms/Lunar Lander (Udo Pernisz, 1979).ch8
5 - game-roms/Most Dangerous Game [Peter Maruhnic].ch8
6 - game-roms/Pong (1 player).ch8
7 - game-roms/Reversi [Philip Baltzer].ch8
8 - game-roms/Space Invaders [David Winter].ch8
9 - game-roms/Tank.ch8
10 - game-roms/Tetris [Fran Dachille, 1991].ch8
11 - test-roms/BC_test.ch8
12 - test-roms/IBM Logo.ch8
13 - test-roms/Keypad Test [Hap, 2006].ch8
14 - test-roms/test_opcode.ch8
Selection [enter a number]: 10
Loading game-roms/Tetris [Fran Dachille, 1991].ch8
File has size of 494 bytes
File read succesfully. 494 bytes read.
First 16 bits: ó ┤ # µ " ╢ p ☺ ╨ ◄ 0 % ↕ ♠ q  
```

## Controls
### Keypad
CHIP-8 uses a 16 button keypad for input. I mapped the 16 keys the following way. 

| Your Keyboard | CHIP-8 Mapping|     
| ------------- |:-------------:|
| `X`           | 0             |
| `1`           | 1             |  
| `2`           | 2             |  
| `3`           | 3             |
| `Q`           | 4             |  
| `W`           | 5             |  
| `E`           | 6             |
| `A`           | 7             |  
| `S`           | 8             |  
| `D`           | 9             |
| `Z`           | A             |
| `C`           | B             |  
| `4`           | C             |  
| `R`           | D             |  
| `F`           | E             |  
| `V`           | F             |  

### Emulator state
The following keys control the emulator state. 
| Key           | Function      |     
| ------------- |:-------------:|
| `F10`         | Pause         |
| `F11`         | Reset         |  
| `F12`         | Quit          |  

#### Pause screen
Pausing the game stops execution of CPU instructions. 
| Game in play state | Game in pause state |     
| -------------      |:-------------:|
| ![image](https://github.com/nicholaswile/CHIP-8-Emulator/assets/74445404/6956ca1e-70a4-42ed-aa2f-18f5a0df13f5) | ![image](https://github.com/nicholaswile/CHIP-8-Emulator/assets/74445404/4e9be257-c8ca-458f-86cb-eb44a275c94c) |


## Future works
I plan to integrate sound processing. I would also like to improve input processing, as the keys can feel "sticky". I've been troubleshooting whether this is a bug with my CHIP-8 CPU instructions or a defect caused by my SDL2 code, but my emulator passes the test cases from the test ROMs. I'd also like to add color customizations. 



