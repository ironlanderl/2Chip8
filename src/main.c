#include "cpu.h"

int randomBool()
{
    return rand() % 2;
}

uint16_t GetKeyBit(SDL_Event* event){
    // Convert 1234qwerasdfzxcv to an index
    switch (event->key.keysym.sym)
    {
    case SDLK_1:
        return 0;
    case SDLK_2:
        return 1;
    case SDLK_3:
        return 2;
    case SDLK_4:
        return 3;
    case SDLK_q:
        return 4;
    case SDLK_w:
        return 5;
    case SDLK_e:
        return 6;
    case SDLK_r:
        return 7;
    case SDLK_a:
        return 8;
    case SDLK_s:
        return 9;
    case SDLK_d:
        return 10;
    case SDLK_f:
        return 11;
    case SDLK_z:
        return 12;
    case SDLK_x:
        return 13;
    case SDLK_c:
        return 14;
    case SDLK_v:
        return 15;
    default:
        return 0;
    }
}



int main(int argc, char *argv[])
{
    //char *filename = "c:\\Users\\fabri\\Downloads\\chip8-test-suite.ch8";
    // char* filename = "C:\\Users\\fabri\\source\\2Chip8\\roms\\ibm.ch8";
    char* filename = "C:\\path\\chip81.ch8";
    InitializeScreen();
    InitializeOtherStuff();
    LoadRom(filename);
    

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
            case SDL_KEYUP:
            case SDL_KEYDOWN:
                KeyPressed(GetKeyBit(&e));
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
        update_timers();
        MainLoop();
        SDL_RenderPresent(renderer);
    }

    return 0;
}