#include "cpu.h"

uint8_t RAM[4096];
uint16_t PC = 0x200;
uint16_t I;
uint8_t V[16];
uint16_t stack[16];
uint16_t stack_top = 0;
char instruction[256];
uint16_t opcode;
bool keys[16];

uint8_t delay_timer;
uint8_t sound_timer;
time_t timer_last_update;

//uint64_t cycles = 0;

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
    memset(&V, 0, sizeof(V));
    memset(&stack, 0, sizeof(stack));
    memset(&keys, false, sizeof(keys));
    LoadFont();
    srand(time(NULL)); // Seed the random number generator with current time
    // RAM[0x1FF] = 0x2;
}

void MainLoop()
{
    Fetch();
    Execute();
}

void Fetch()
{
    opcode = RAM[PC] << 8 | RAM[PC + 1];
    // printf("%X: %X - %X\n", PC, RAM[PC], RAM[PC + 1]);
    PC += 2;
    //cycles++;
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
    // print all keys state in one line
    for (int i = 0; i < 16; i++)
    {
        printf("%d ", keys[i]);
    }
    printf("\n");

    bool shouldSet;
    
    switch (A)
    {
    case 0x0:
        switch (NN)
        {
        case 0xE0:
            InitializeScreen();
            break;
        case 0xEE:
            stack_top--;
            PC = stack[stack_top];
            break;
        default:
            printf("Undefined Opcode: %X%X%X%X\n", A, X, Y, N);
            exit(-1);
            break;
        }
        break;
    case 0x1:
        PC = NNN;
        break;
    case 2:
        stack[stack_top++] = PC;
        PC = NNN;
        break;
    case 0x3:
        if (V[X] == NN)
        {
            PC += 2;
        }
        break;
    case 0x4:
        if (V[X] != NN)
        {
            PC += 2;
        }
        break;
    case 0x5:
        if (V[X] == V[Y])
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
    case 8:
        switch (N)
        {
        case 0:
            V[X] = V[Y];
            break;
        case 1:
            V[X] = V[X] | V[Y];
            break;
        case 2:
            V[X] = V[X] & V[Y];
            break;
        case 3:
            V[X] = V[X] ^= V[Y];
            break;
        case 4:
            shouldSet = (V[X] + V[Y] > 255) ? 1 : 0;
            V[X] += V[Y];
            // Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there is not.
            V[0xF] = shouldSet;
            break;
        case 5:
            shouldSet = (V[X] > V[Y]) ? 1 : 0;
            V[X] -= V[Y];
            // VY is subtracted from VX. VF is set to 1 when there's a borrow, and 0 when there is not.
            V[0xF] = shouldSet;
            break;
        case 6:
            uint8_t lsb = V[X] & 0b1;
            // Move V[X] to the right
            V[X] >>= 1;
            // Save LSB
            V[0xF] = lsb;
            break;
        case 7:
            V[X] = V[Y] - V[X];
            V[0xF] = (V[X] < V[Y]) ? 1 : 0;
            break;
        case 0xE:
            uint8_t msb = V[X] & 0b10000000;
            // Move the msb bit to be the first
            msb >>= 7;
            // Move V[X] to the left
            V[X] <<= 1;
            V[0xF] = msb;
            break;
        default:
            printf("Undefined Opcode: %X%X%X%X\n", A, X, Y, N);
            break;
        }
        break;
    case 0x9:
        if (V[X] != V[Y])
            PC += 2;
        break;
    case 0xA:
        I = NNN;
        break;
    case 0xB:
        PC = V[0] + NNN;
        break;
    case 0xC:
        V[X] = rand() % 256 & NN;
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
        switch (NN)
        {
        case 0x9E:
            if (keys[V[X]])
                PC += 2;
            break;
        case 0xA1:
            if (!keys[V[X]])
                PC += 2;
            break;
        }
        break;

    case 0xF:
        switch (NN)
        {
        case 0x07:
            V[X] = delay_timer;
            break;
        case 0xA:
            uint16_t key = -1;
            for (int i = 0; i < 0x16; i++)
            {
                if (keys[i])
                {
                    key = i;
                    break;
                }
            }
            if (key == -1)
            {
                PC -= 2;
                break;
            }
            V[X] = key;
            break;
        case 0x15:
            delay_timer = V[X];
            break;
        case 0x18:
            sound_timer = V[X];
            break;
        case 0x1E:
            I += V[X];
            break;
        case 0x29:
            I = V[X] * 5;
            break;
        case 0x33:
            uint8_t value = V[X];
            // Extract the BCD digits
            uint8_t hundreds = value / 100;
            uint8_t tens = (value / 10) % 10;
            uint8_t ones = value % 10;

            // Store the BCD digits in memory
            RAM[I] = hundreds;
            RAM[I + 1] = tens;
            RAM[I + 2] = ones;
            break;
        case 0x55:
            for (size_t i = 0; i <= X; i++)
            {
                RAM[I + i] = V[i];
            }
            break;
        case 0x65:
            for (size_t i = 0; i <= X; i++)
            {
                V[i] = RAM[I + i];
            }
            break;
        default:
            printf("Undefined Opcode: %X%X%X%X\n", A, X, Y, N);
            exit(-1);
            break;
        }
        break;

    default:
        printf("Undefined Opcode: %X%X%X%X\n", A, X, Y, N);
        exit(-1);
        break;
    }
}

void KeyPressed(uint16_t index)
{
    // Xor the keys[index] with true
    keys[index] ^= true;
}

void update_timers()
{
    // Get the current time
    time_t current_time = time(NULL);

    // Calculate the elapsed time since the last update
    double elapsed_time = difftime(current_time, timer_last_update);

    // Calculate the number of timer decrements that should have happened in the elapsed time
    int decrements = elapsed_time * TIMER_FREQUENCY;

    // Decrement the delay timer
    if (delay_timer > 0)
    {
        delay_timer -= decrements;
        if (delay_timer < 0)
            delay_timer = 0;
    }

    // Decrement the sound timer
    if (sound_timer > 0)
    {
        sound_timer -= decrements;
        if (sound_timer < 0)
            sound_timer = 0;
    }

    // Update the last update time
    timer_last_update = current_time;

    // Make the computer "beep" if the sound timer is above 0
    if (sound_timer > 0)
        printf("\a");
}