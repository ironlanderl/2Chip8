#include "cpu.h"

uint8_t RAM[4096];
uint16_t PC = 0x200;
uint16_t I;
uint8_t V[16];
uint16_t stack[USHRT_MAX];
uint16_t stack_top = 0;
char instruction[256];
uint16_t opcode;
uint16_t keyboard;

uint64_t cycles = 0;

bool screen[ALTEZZA][LUNGHEZZA];

void GetScreen(bool *output)
{
    memcpy(output, screen, sizeof(screen));
}

void LoadRom(char *filename)
{
    InitializeOtherStuff();
    FILE *rom_file = fopen(filename, "rb");
    if (rom_file == NULL)
    {
        perror("Error opening ROM file");
        exit(1);
    }

    // Find the starting address in RAM to load the ROM data
    int j = 0x200;

    // Read the ROM data from the file into a buffer
    fseek(rom_file, 0L, SEEK_END);
    size_t rom_size = ftell(rom_file);
    rewind(rom_file);
    unsigned char *rom_buffer = malloc(rom_size);
    fread(rom_buffer, rom_size, 1, rom_file);

    // Copy the ROM data to the RAM starting at address 0x200
    memcpy(&RAM[j], rom_buffer, rom_size);

    // Free the buffer and close the file
    free(rom_buffer);
    fclose(rom_file);
}

void LoadFont()
{
    uint8_t fontset[] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };
    memcpy(&RAM[0x50], fontset, sizeof(fontset));
}

void InitializeScreen()
{
    memset(&screen, false, sizeof(screen));
}

void InitializeOtherStuff()
{
    memset(&RAM, 0, sizeof(RAM));
    RAM[0x1FF] = 0x2;
}

void MainLoop()
{
    Fetch();
    Execute();
}

void Fetch()
{
    opcode = RAM[PC] << 8 | RAM[PC + 1];
    printf("%X: %X - %X\n", PC, RAM[PC], RAM[PC + 1]);
    PC += 2;
    cycles++;
}

void Execute()
{
    uint16_t A = (opcode & 0xF000) >> 12;
    uint16_t X = (opcode & 0x0F00) >> 8;
    uint16_t Y = (opcode & 0x00F0) >> 4;
    uint16_t N = opcode & 0x000F;
    uint16_t NN = opcode & 0x00FF;
    uint16_t NNN = opcode & 0x0FFF;

    printf("A: %X, X: %X, Y: %X, N: %X, NN: %X, NNN: %X\n", A, X, Y, N, NN, NNN);

    switch (A)
    {
    case 0x0:
        if (opcode == 0x00E0)
            InitializeScreen();
        if (opcode == 0x00EE)
            PC = stack[stack_top--];
        break;
    case 0x1:
        PC = NNN;
        break;
    case 2:
        stack[stack_top++] = PC;
        PC = NNN;
        break;
    case 0x4:
        if (V[X] != NN)
        {
            PC += 2;
        }
        break;
    case 0x6:
        V[X] = NN;
        break;
    case 0x7:
        V[X] += NN;
        break;
    case 0xA:
        I = NNN;
        break;

    case 0xD:
    {
        // debugger;
        //  Prendo le coordinate dove disegnare
        int x = V[X] % LUNGHEZZA;
        int y = V[Y] % ALTEZZA;
        // Resetto V[0xF]
        // Segna se un bit è stato sovrascritto
        V[0xF] = 0;

        // altezza
        for (int i = 0; i < N; i++)
        {
            // prendo i dati per la riga 'i' dello sprite
            // let data = RAM[I + i];
            // scrivo allo schermo in base alla lunghezza fissa di 8bit per riga
            for (int j = 0; j < 8; j++)
            {
                // Controllo se esco dallo schermo
                if (x + j < LUNGHEZZA)
                {
                    int data = (RAM[I + i] & (1 << (7 - j))) ? 1 : 0;
                    // controllo se il bit è 1
                    if (data)
                    {
                        if (screen[y + i][x + j] == false)
                        {
                            screen[y + i][x + j] = true;
                        }
                        else
                        {
                            screen[y + i][x + j] = false;
                            V[0xF] = 0x1;
                        }
                    }
                }
            }
        }
    }
    break;

    case 0xE:
        

    case 0xF:
        if (NN == 0x65)
        {
            for (size_t i = 0; i < X; i++)
            {
                V[X] = RAM[I + i];
            }
        }
        break;

    default:
        printf("Undefined Opcode: %X%X%X%X\n", A, X, Y, N);
        exit(-1);
        break;
    }
}

void KeyChange(uint16_t key)
{
    keyboard = keyboard ^ key;
}
