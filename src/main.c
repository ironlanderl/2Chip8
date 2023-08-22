#include "cpu.h"

int clocks = 8;

int randomBool()
{
    return rand() % 2;
}

long long current_timestamp() {
    struct timeval te; 
    gettimeofday(&te, NULL); // get current time
    long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000; // calculate milliseconds
    // printf("milliseconds: %lld\n", milliseconds);
    return milliseconds;
}

void handleHotkeys(SDL_Event *event){
    switch (event->key.keysym.sym)
    {
    case SDLK_F2:
        clocks++;
        break;
    case SDLK_F1:
        if (clocks > 1)
        {
            clocks--;
        }
        break;
    default:
        break;
    }

}

uint16_t GetKeyBit(SDL_Event *event)
{
    // Convert 1234qwerasdfzxcv to an index
    switch (event->key.keysym.sym)
    {
    case SDLK_1:
        return 1;
    case SDLK_2:
        return 2;
    case SDLK_3:
        return 3;
    case SDLK_4:
        return 0xC;
    case SDLK_q:
        return 4;
    case SDLK_w:
        return 5;
    case SDLK_e:
        return 6;
    case SDLK_r:
        return 0xD;
    case SDLK_a:
        return 7;
    case SDLK_s:
        return 8;
    case SDLK_d:
        return 9;
    case SDLK_f:
        return 0xE;
    case SDLK_z:
        return 0xA;
    case SDLK_x:
        return 0;
    case SDLK_c:
        return 0xB;
    case SDLK_v:
        return 0xF;
    default:
        return NULL;
    }
}

int main(int argc, char *argv[])
{
    char *filename = "/home/ironlanderl/src/2Chip8/roms/br8kout.ch8";
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
    long long lastTime, startTime;
    while (running)
    {

        // https://www.reddit.com/r/EmuDev/comments/en0s6t/chip8_500_hz_feels_too_fast/
        startTime = current_timestamp();
        for (int i = 0; i < clocks; i++)
        {
            MainLoop();
        }

        update_timers();


        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            switch (e.type)
            {
            case SDL_KEYDOWN:
                KeyPressed(GetKeyBit(&e));
                handleHotkeys(&e);
                break;
            case SDL_KEYUP:
                KeyReleased(GetKeyBit(&e));
                handleHotkeys(&e);
                break;
            case SDL_QUIT:
                running = 0;
                break;
            }
        }

        if (NeedToRefresh())
        {
            SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
            SDL_RenderClear(renderer);
            SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);

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
            SDL_RenderPresent(renderer);
        }

        long long elapsed = current_timestamp() - startTime;

        SDL_Delay((1.0/60)*1000 - elapsed);
        char title[256];
        sprintf(title, "2Chip8 - Clocks: %d", clocks);
        SDL_SetWindowTitle(window, title);
        
    }

    return 0;
}