#include <SDL2/sdl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

#define LUNGHEZZA 64
#define ALTEZZA 32

void GetScreen(bool* output);
void InitializeScreen();

void LoadRom(char *filename);
void LoadFont();

void MainLoop();
void Fetch();
void Execute();

void KeyChange(uint16_t key);