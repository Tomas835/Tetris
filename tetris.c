#include "tetris.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

const int objekty[7][16] = 
{
    {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},  
    {0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0},  
    {0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0},  
    {0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},  
    {0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0},  
    {0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0}, 
    {0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0}  
};

int arena[VYSKA][SIRKA];
uint32_t score = 0;
uint32_t radkyOdstranene = 0;
bool konecHry = false;
int aktualniTetrisIndex = 0;
int aktualniRotace = 0;
int poziceX = VYSKA / 2;
int poziceY = 0;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

uint32_t getMs(void) 
{
    return SDL_GetTicks();
}

bool pozice(int tetris, int rotate, int posX, int posY) 
{
    for (int x = 0; x < objektSIRKA; x++) 
    {
        for (int y = 0; y < objektVYSKA; y++) 
        {
            int index = rotace(x, y, rotate); 
            if (objekty[tetris][index] == 0) continue;

            int arenaX = x + posX;
            int arenaY = y + posY;

            if (arenaX < 0 || arenaX >= SIRKA || arenaY >= VYSKA) return false;
            if (arenaY >= 0 && arena[arenaY][arenaX] == 1) return false;
        }
    }
    return true;
}

int rotace(int x, int y, int rotace) 
{
    switch (rotace % 4) 
    {
        case 0: return x + y * objektSIRKA;
        case 1: return 12 + y - (x * objektSIRKA);
        case 2: return 15 - (y * objektSIRKA) - x;
        case 3: return 3 - y + (x * objektSIRKA);
        default: return 0;
    }
}

void novyObjekt(void) 
{
    aktualniTetrisIndex = rand() % 7;
    aktualniRotace = 0;
    poziceX = SIRKA / 2 - objektSIRKA / 2;
    poziceY = 0;

    if (!pozice(aktualniTetrisIndex, aktualniRotace, poziceX, poziceY)) 
    {
        printf("Game Over!\n");
        konecHry = true;
    }
}

void processInputs(SDL_Event *event) 
{
    int dalsiRotace;
    if (event->type == SDL_KEYDOWN) 
    {
        switch (event->key.keysym.sym) 
        {
            case SDLK_SPACE:
                dalsiRotace = (aktualniRotace + 1) % 4;
                if (pozice(aktualniTetrisIndex, dalsiRotace, poziceX, poziceY)) 
                {
                    aktualniRotace = dalsiRotace;
                }
                break;
            case SDLK_LEFT:
                if (pozice(aktualniTetrisIndex, aktualniRotace, poziceX - 1, poziceY)) 
                {
                    poziceX--;
                }
                break;
            case SDLK_RIGHT:
                if (pozice(aktualniTetrisIndex, aktualniRotace, poziceX + 1, poziceY)) 
                {
                    poziceX++;
                }
                break;
            case SDLK_DOWN:
                pohybDolu();
                break;
        }
    }
}

bool pohybDolu(void) 
{
    if (pozice(aktualniTetrisIndex, aktualniRotace, poziceX, poziceY + 1)) 
    {
        poziceY++;
        return true;
    }

    pridatDoHry();
    kontrolaLajn();
    novyObjekt();
    return false;
}

void pridatDoHry(void) 
{
    for (int y = 0; y < objektVYSKA; y++) 
    {
        for (int x = 0; x < objektSIRKA; x++) 
        {
            int index = rotace(x, y, aktualniRotace); 
            if (objekty[aktualniTetrisIndex][index] == 1) 
            {
                int arenaX = poziceX + x;
                int arenaY = poziceY + y;
                if (arenaX >= 0 && arenaX < SIRKA && arenaY >= 0 && arenaY < VYSKA) 
                {
                    arena[arenaY][arenaX] = 1;
                }
            }
        }
    }
}

void kontrolaLajn(void) 
{
    int vycisteniLajn = 0;

    for (int y = VYSKA - 1; y >= 0; y--) 
    {
        bool plnaLajna = true;
        for (int x = 0; x < SIRKA; x++) 
        {
            if (arena[y][x] == 0) 
            {
                plnaLajna = false;
                break;
            }
        }

        if (plnaLajna) 
        {
            vycisteniLajn++;
            
            for (int yy = y; yy > 0; yy--) 
            {
                for (int xx = 0; xx < SIRKA; xx++) 
                {  
                    arena[yy][xx] = arena[yy - 1][xx];
                }
            }

        
            for (int xx = 0; xx < SIRKA; xx++) 
            {
                arena[0][xx] = 0;
            }

            y++;  
        }
    }

    if (vycisteniLajn > 0) 
    {
        score += 100 * vycisteniLajn;
        radkyOdstranene += vycisteniLajn;
    }
}

void kresleniHry(void) 
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  
    SDL_RenderClear(renderer);

    for (int y = 0; y < VYSKA; y++) 
    {
        for (int x = 0; x < SIRKA; x++) 
        {
            if (arena[y][x] == 1) 
            {
                SDL_Rect rect = {x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE};
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);  
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }

    for (int y = 0; y < objektVYSKA; y++) 
    {
        for (int x = 0; x < objektSIRKA; x++) 
        {
            int index = rotace(x, y, aktualniRotace); 
            if (objekty[aktualniTetrisIndex][index] == 1) 
            {
                SDL_Rect rect = {(poziceX + x) * BLOCK_SIZE, (poziceY + y) * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE};
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);  
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }

    vypsaniSkore();
    SDL_RenderPresent(renderer);
}

void vypsaniSkore(void) 
{
    char scoreText[32];
    snprintf(scoreText, sizeof(scoreText), "Score: %d", score);
    char lajnyText[32];
    snprintf(lajnyText, sizeof(lajnyText), "Lines: %d", radkyOdstranene);

    printf("%s\n%s\n", scoreText, lajnyText);
}

int main(void) 
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) 
    {
        printf("SDL_Init error: %s\n", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SIRKA * BLOCK_SIZE, VYSKA * BLOCK_SIZE, SDL_WINDOW_SHOWN);
    if (!window) 
    {
        printf("SDL_CreateWindow error: %s\n", SDL_GetError());
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) 
    {
        printf("SDL_CreateRenderer error: %s\n", SDL_GetError());
        return 1;
    }

    srand((unsigned int)time(NULL));

    memset(arena, 0, sizeof(arena));
    novyObjekt();

    SDL_Event event;
    uint32_t lastTime = getMs();

    while (!konecHry) 
    {
        while (SDL_PollEvent(&event)) 
        {
            if (event.type == SDL_QUIT) 
            {
                konecHry = true;
            } 
            else 
            {
                processInputs(&event);
            }
        }

        uint32_t currentTime = getMs();
        if (currentTime - lastTime > 500) 
        {
            lastTime = currentTime;
            if (!pohybDolu()) {
            }
        }

        kresleniHry();
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
