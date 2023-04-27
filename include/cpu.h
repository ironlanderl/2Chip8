#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <time.h>   // For time()

#define TIMER_FREQUENCY 60 //HZ


#define LUNGHEZZA 64
#define ALTEZZA 32

void GetScreen(bool* output);
void InitializeScreen();

void LoadRom(char *filename);
void LoadFont();

void MainLoop();
void Fetch();
void Execute();

void KeyPressed(uint16_t key);

void InitializeOtherStuff();
void update_timers();