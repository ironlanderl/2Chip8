#include "cpu.h"

int randomBool()
{
    return rand() % 2;
}

uint16_t GetKeyBit(SDL_Event* event){
    // Convert 1234qwerasdfzxcv to a single bit in a 16 bit number
    switch (event->key.keysym.sym)
    {
    case SDLK_1:
        return 0x0001;
    case SDLK_2:
        return 0x0002;
    case SDLK_3:
        return 0x0004;
    case SDLK_4:
        return 0x0008;
    case SDLK_q:
        return 0x0010;
    case SDLK_w:
        return 0x0020;
    case SDLK_e:
        return 0x0040;
    case SDLK_r:
        return 0x0080;
    case SDLK_a:
        return 0x0100;
    case SDLK_s:
        return 0x0200;
    case SDLK_d:
        return 0x0400;
    case SDLK_f:
        return 0x0800;
    case SDLK_z:
        return 0x1000;
    case SDLK_x:
        return 0x2000;
    case SDLK_c:
        return 0x4000;
    case SDLK_v:
        return 0x8000;
    default:
        return 0x0000;
    }
}


int main(int argc, char *argv[])
{
    char *filename = "c:\\Users\\fabri\\Downloads\\chip8-test-suite.ch8";
    // char* filename = "C:\\Users\\fabri\\source\\2Chip8\\ibm.ch8";
    LoadRom(filename);
    LoadFont();
    InitializeScreen();

    SDL_Window *window = SDL_CreateWindow("2Chip8",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          640,
                                          320,
                                          SDL_WINDOW_SHOWN);

    SDL_Renderer *renderer = SDL_CreateRenderer(window,
                                                -1,
                                                SDL_RENDERER_ACCELERATED |
                                                    SDL_RENDERER_PRESENTVSYNC);

    SDL_RenderSetIntegerScale(renderer, SDL_TRUE);
    SDL_RenderSetScale(renderer, 10, 10);
    int running = 1;
    while (running)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            switch (e.type)
            {
            case SDL_KEYDOWN:

                break;
            case SDL_QUIT:
                running = 0;
                break;
            }
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        bool screen[ALTEZZA][LUNGHEZZA];
        GetScreen(screen);

        for (int i = 0; i < 32; i++)
        {
            for (int j = 0; j < 64; j++)
            {
                if (screen[i][j])
                {
                    SDL_RenderDrawPoint(renderer, j, i);
                }
            }
        }
        MainLoop();
        SDL_RenderPresent(renderer);
    }

    return 0;
}