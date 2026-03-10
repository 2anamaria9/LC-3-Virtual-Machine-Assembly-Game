#include "lc3.h"
#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define WIDTH 64
#define HEIGHT 64
#define PIXEL_SCALE 8
#define VRAM_START 0xC000

vm_state_t vm;

void (*opcodes[16])(uint16_t) = {
    br, add, ld, st, jsr, andd, ldr, str, 
    rti_or_res, nott, ldi, sti, jmp, rti_or_res, lea, trap
};

int read_image(const char* path) {
    FILE* f = fopen(path, "rb");
    if (!f) {
        return 0;
    }
    uint16_t start;
    fread(&start, sizeof(start), 1, f);
    start = swap16(start);
    uint16_t* p = vm.memory + start;
    size_t read = fread(p, sizeof(uint16_t), MEMORY_MAX - start, f);
    for (size_t i = 0; i < read; i++) {
        p[i] = swap16(p[i]);
    }
    fclose(f);
    return 1;
}

int main(int argc, const char* argv[]) {
    if (argc < 2) {
        return 2;
    }
    if (!read_image(argv[1])) {
        return 1;
    }
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Flappy Pixel | Score: 0",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WIDTH * PIXEL_SCALE,
        HEIGHT * PIXEL_SCALE,
        0
    );
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture* texture = SDL_CreateTexture(
        renderer, 
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
        WIDTH, HEIGHT
    );
    uint32_t pixel_buffer[WIDTH * HEIGHT];
    vm.reg[R_COND] = FL_ZRO;
    vm.reg[R_PC] = 0x3000;
    vm.running = 1;
    SDL_Event event;
    while (vm.running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                vm.running = 0;
            } 
            else if (event.type == SDL_KEYDOWN) {
                vm.memory[MR_KBSR] = 0x8000;
                int key = event.key.keysym.sym;
                vm.memory[MR_KBDR] = key; 
            }
        }
        for (int i = 0; i < 5000; i++) {
            if (!vm.running) {
                break;
            }
            uint16_t current_pc = vm.reg[R_PC];
            uint16_t instruction = mem_read(current_pc);
            vm.reg[R_PC]++;
            uint16_t operation = instruction >> 12;
            opcodes[operation](instruction);
            if (vm.reg[R_PC] == 0x3000) {
                break;
            }
        }
        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                uint16_t vram_data = vm.memory[VRAM_START + (y * WIDTH) + x];
                uint8_t r = ((vram_data >> 10) & 0x1F) << 3;
                uint8_t g = ((vram_data >> 5) & 0x1F) << 3;
                uint8_t b = (vram_data & 0x1F) << 3;
                pixel_buffer[y * WIDTH + x] = (r << 24) | (g << 16) | (b << 8) | 255;
            }
        }
        SDL_UpdateTexture(texture, NULL, pixel_buffer, WIDTH * sizeof(uint32_t));
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
        uint16_t score = vm.memory[0x4000];
        char title[50];
        sprintf(title, "Flappy Pixel | Score: %d", score);
        SDL_SetWindowTitle(window, title);
        SDL_Delay(16);
    }
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}