#include "../headers/chip8.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <SDL2/SDL.h>

const int UPSCALE = 20;
const int WIDTH = 64*UPSCALE, HEIGHT = 32*UPSCALE;
const int FPS = 60;
const int FRAME_TIME = 1000/FPS;
const int NUM_CYCLES = 700/FPS; // cycles per frame
const char* TITLE = "Niko's CHIP-8 Emulator";
const char* CONFIG_PATH = "config.txt";

enum state {START=0, PLAY, PAUSE, RESET, QUIT};
state GAMESTATE = START;

void play_loop(chip8 &chip8, SDL_Renderer *renderer);
void pause_loop(chip8 &chip8, SDL_Renderer *renderer);
void reset(chip8 &chip8);
bool load_game(chip8 &chip8);
void get_input(chip8 &chip8);
void display_graphics(BYTE *display, SDL_Renderer *renderer);
void display_graphics(std::string &display, SDL_Renderer *renderer);
void sync_time(chip8 &chip8);

std::string title_screen;
std::string pause_screen;

int main (int argc, char** argv) {
    // Not used right now. 
    title_screen =  "................................................................";
    title_screen += "................................................................";
    title_screen += "....######......####....####....####...##...############........";
    title_screen += "....########....####....####....####..##....####....####........";
    title_screen += "....####..####..####............#######.....####....####........";
    title_screen += "....####....########....####....#######.....####....####........";
    title_screen += "....####......######....####....####..##....####....####........";
    title_screen += "....####........####....####....####...##...############........";
    title_screen += "................................................................";
    title_screen += "................................................................";
    title_screen += "................................................................";
    title_screen += "......................................................####......";
    title_screen += "....########....####....####....####....##########...##...#.....";
    title_screen += "....####........####....####....####....####....##...##...#.....";
    title_screen += "....####........############............##########....####......";
    title_screen += "....####........############....####....####.........##...#.....";
    title_screen += "....####........####....####....####....####.........##...#.....";
    title_screen += "....########....####....####....####....####..........####......";
    title_screen += "................................................................";
    title_screen += "................................................................";
    title_screen += "................................................................";
    title_screen += "....................####....####................................";
    title_screen += "....................####....####.......####..#####.####.#..#....";
    title_screen += "..........................................#..#...#....#.#..#....";
    title_screen += ".........................................#...#...#...#..####....";
    title_screen += "..................,##..........##......####..#####.####....#....";
    title_screen += "...................##..........##...............................";
    title_screen += ".....................##......##.................................";
    title_screen += ".......................######...................................";
    title_screen += "................................................................";
    title_screen += "................................................................";
    title_screen += "................................................................";

    pause_screen =  "................................................................";
    pause_screen += "................................................................";
    pause_screen += "................................................................";
    pause_screen += "................................................................";
    pause_screen += "................................................................";
    pause_screen += "................................................................";
    pause_screen += "................................................................";
    pause_screen += "................................................................";
    pause_screen += "................................................................";
    pause_screen += "................................................................";
    pause_screen += "........#######.#######.##...##.########.#####.#####............";
    pause_screen += "........##...##.##...##.##...##.##.......##....##...##..........";
    pause_screen += "........#######.#######.##...##...##.....#####.##....##.........";
    pause_screen += "........##......##...##.##...##.....##...##....##....##.........";
    pause_screen += "........##......##...##.##...##.......##.##....##...##..........";
    pause_screen += "........##......##...##.#######.########.#####.#####............";
    pause_screen += "................................................................";
    pause_screen += "................................................................";
    pause_screen += "................................................................";
    pause_screen += "............................##....##............................";
    pause_screen += "............................##....##............................";
    pause_screen += "............................##....##............................";
    pause_screen += "............................##....##............................";
    pause_screen += "............................##....##............................";
    pause_screen += "............................##....##............................";
    pause_screen += "................................................................";
    pause_screen += "................................................................";
    pause_screen += "................................................................";
    pause_screen += "................................................................";
    pause_screen += "................................................................";
    pause_screen += "................................................................";
    pause_screen += "................................................................";
    
    chip8 chip8;
    load_game(chip8) ? GAMESTATE=PLAY : GAMESTATE=QUIT;

    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer);
    SDL_RenderSetScale(renderer, UPSCALE, UPSCALE);
    SDL_SetWindowTitle(window, TITLE);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    if (window == NULL) {
        std::cerr << "Could not create window: " << SDL_GetError() << "\n";
        return 1;
    }

    SDL_Event windowEvent;

    bool running = true;
    while (running) {
        int frame_time_start = SDL_GetTicks();

        switch (GAMESTATE) {
            case START:                                     break;
            case PLAY:      play_loop(chip8, renderer);     break;
            case PAUSE:     pause_loop(chip8, renderer);    break;
            case RESET:     reset(chip8);                   break;
            case QUIT:      running=false;                  break;
            default:                                        break;
        }

        if (SDL_PollEvent(&windowEvent)) {
            if (windowEvent.type == SDL_QUIT )
                break;
        }

        int frame_delta_time = SDL_GetTicks() - frame_time_start;
        if (frame_delta_time < FRAME_TIME) 
            SDL_Delay(FRAME_TIME-frame_delta_time);
        
    }
    return 0;
}

void play_loop(chip8 &chip8, SDL_Renderer *renderer) {
    get_input(chip8);

    for (int i = 0; i < NUM_CYCLES; i++)
        chip8.cycle();

    display_graphics(chip8.display, renderer);   
    sync_time(chip8);
}

void pause_loop(chip8 &chip8, SDL_Renderer *renderer) {
    get_input(chip8);
    display_graphics(pause_screen, renderer);
}

void reset(chip8 &chip8) {
    chip8.reset();
    GAMESTATE = PLAY;
}

bool load_game(chip8 &chip8) {
    std::vector<std::string> roms;
    std::ifstream config_file(CONFIG_PATH);
    if (!config_file.is_open()) {
        std::cerr << "Error: could not open config file.\n";
        return false;
    }
    std::string line;
    while (std::getline(config_file, line)) {
        roms.push_back(line);
    }
    config_file.close();
    std::cout << "Select a ROM to play on CHIP-8!\n";
    for (int i = 0; i < roms.size(); i++) 
        std::cout << i << " - " << roms[i] << "\n";
    int user_selection;
    std::cout << "Selection [enter a number]: ";
    do {
        std::cin >> user_selection;
        if (user_selection >= roms.size()) {
            std::cout << "Error: invalid selection.\n";
        }
    } while (user_selection >= roms.size());
    const std::string game = roms[user_selection];
    chip8.init(game);
    return true;
}

/* Keyboard
    1	2	3	C
    4	5	6	D
    7	8	9	E
    A	0	B	F
*/
void get_input(chip8 &chip8) {
    for (int i = 0; i < 16; i++)
        chip8.keys[i] = false;
    
    SDL_Event keyboardEvent;
    if (SDL_PollEvent(&keyboardEvent)) {
        
        switch (keyboardEvent.type) {
            case SDL_KEYDOWN:   
            switch (keyboardEvent.key.keysym.sym) {
                case SDLK_x:    chip8.keys[0x0] = true;            break;
                case SDLK_1:    chip8.keys[0x1] = true;            break;
                case SDLK_2:    chip8.keys[0x2] = true;            break;
                case SDLK_3:    chip8.keys[0x3] = true;            break;
                case SDLK_q:    chip8.keys[0x4] = true;            break;
                case SDLK_w:    chip8.keys[0x5] = true;            break;
                case SDLK_e:    chip8.keys[0x6] = true;            break;
                case SDLK_a:    chip8.keys[0x7] = true;            break;
                case SDLK_s:    chip8.keys[0x8] = true;            break;
                case SDLK_d:    chip8.keys[0x9] = true;            break;
                case SDLK_z:    chip8.keys[0xA] = true;            break;
                case SDLK_c:    chip8.keys[0xB] = true;            break;
                case SDLK_4:    chip8.keys[0xC] = true;            break;
                case SDLK_r:    chip8.keys[0xD] = true;            break;
                case SDLK_f:    chip8.keys[0xE] = true;            break;
                case SDLK_v:    chip8.keys[0xF] = true;            break;
                case SDLK_F10:    
                GAMESTATE=(GAMESTATE==PLAY?PAUSE:PLAY);            break;
                case SDLK_F11:
                GAMESTATE=RESET;                                   break;
                case SDLK_F12:
                GAMESTATE=QUIT;                                    break;
                default:                                           break;
            }     
            break; 

            case SDL_KEYUP:     
            switch (keyboardEvent.key.keysym.sym) {
                case SDLK_x:    chip8.keys[0x0] = false;           break;
                case SDLK_1:    chip8.keys[0x1] = false;           break;
                case SDLK_2:    chip8.keys[0x2] = false;           break;
                case SDLK_3:    chip8.keys[0x3] = false;           break;
                case SDLK_q:    chip8.keys[0x4] = false;           break;
                case SDLK_w:    chip8.keys[0x5] = false;           break;
                case SDLK_e:    chip8.keys[0x6] = false;           break;
                case SDLK_a:    chip8.keys[0x7] = false;           break;
                case SDLK_s:    chip8.keys[0x8] = false;           break;
                case SDLK_d:    chip8.keys[0x9] = false;           break;
                case SDLK_z:    chip8.keys[0xA] = false;           break;
                case SDLK_c:    chip8.keys[0xB] = false;           break;
                case SDLK_4:    chip8.keys[0xC] = false;           break;
                case SDLK_r:    chip8.keys[0xD] = false;           break;
                case SDLK_f:    chip8.keys[0xE] = false;           break;
                case SDLK_v:    chip8.keys[0xF] = false;           break;
                default:                                           break;
            }     
            break;

            default:
            break;
        }        
    }
}

void display_graphics(BYTE *display, SDL_Renderer *renderer) {
    for (int x = 0; x < 64; x++) {
        for (int y = 0; y < 32; y++)  {

            // If pixel is 1, paint it white.
            // Else, paint it black.
            int col = display[y*64+x];
            SDL_SetRenderDrawColor(renderer, col*255, col*255, col*255, col*255);
            SDL_RenderDrawPoint(renderer, x, y);
        }
    }
    SDL_RenderPresent(renderer);
}

void display_graphics(std::string &screen, SDL_Renderer *renderer) {
  for (int x = 0; x < 64; x++) {
        for (int y = 0; y < 32; y++)  {
            int col = ((screen[y*64+x] == '#') ? 1 : 0);
            SDL_SetRenderDrawColor(renderer, col*255, col*255, col*255, col*255);
            SDL_RenderDrawPoint(renderer, x, y);
        }
    }
    SDL_RenderPresent(renderer);
}

void sync_time(chip8 &chip8) {
    if (chip8.delay_timer > 0)
        --chip8.delay_timer;

    if (chip8.sound_timer > 0) {
        std::cout << "Beep!\n";
        --chip8.sound_timer;
    }
}
