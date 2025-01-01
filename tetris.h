#ifndef TETRIS_H
#define TETRIS_H

#include <stdbool.h>
#include <stdint.h>
#include <SDL2/SDL.h>

#define SIRKA  10   
#define VYSKA 20   
#define objektSIRKA  4    
#define objektVYSKA 4    
#define BLOCK_SIZE 30 

extern const int tetrisOBJEKTY[7][16];  
extern int arena[VYSKA][SIRKA];  
extern uint32_t score;                
extern uint32_t radkyOdstranene;         
extern bool koneHry;                 
extern int aktualniTetrisIndex;          
extern int aktualniRotace;              
extern int poziceX, poziceY;              

uint32_t getMs(void);
bool pozice(int tetris, int rotace, int posX, int posY);
int rotate(int x, int y, int rotace);
void novyObjekt(void);
void processInputs(SDL_Event *event);
bool pohybDolu(void);
void pridatDoHry(void);
void kontrolaLajn(void);
void kresleniHry(void);
void vypsaniSkore(void);

#endif 
