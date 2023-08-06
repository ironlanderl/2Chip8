#include "cpu.h"

int randomBool()
{
    return rand() % 2;
}

void main_loop_handler(int signum)
{
    MainLoop();
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
        return 0;
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

    struct sigaction sa;
    struct itimerval timer;

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &main_loop_handler;
    sigaction(SIGALRM, &sa, NULL);

    // Configure the timer to interrupt at the desired frequency
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 1000000 / EMULATOR_FREQUENCY;
    timer.it_interval = timer.it_value;
    setitimer(ITIMER_REAL, &timer, NULL);

    int running = 1;
    while (running)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            switch (e.type)
            {
            case SDL_KEYDOWN:
                KeyPressed(GetKeyBit(&e));
                break;
            case SDL_KEYUP:
                KeyReleased(GetKeyBit(&e));
                break;
            case SDL_QUIT:
                running = 0;
                break;
            }
        }

        if (NeedToRefresh())
        {
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
            SDL_RenderPresent(renderer);
        }
        update_timers();
        // MainLoop();
    }

    return 0;
}