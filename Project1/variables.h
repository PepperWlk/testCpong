#pragma once
#ifndef PONG_VARIABLES_H
#define PONG_VARIABLES_H

#include <SDL.h>
#include <stdbool.h>

// Structure pour stocker le contexte SDL
typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* backgroundTexture; // Texture pour l'image de fond
    SDL_Texture* texture;
    SDL_Texture* textureball;
    SDL_Surface* image;
} SDL_Context;

// Variables de jeu
extern int PosxSquareRight;
extern int PosxSquareLeft;
extern int PosySquareRight;
extern int PosySquareLeft;
extern int WidthSquare;
extern int HeightSquare;
extern int PosxMovingSquare;
extern int PosyMovingSquare;
extern int SizeMovingSquare;
extern int PosxMovingSquare2;
extern int PosyMovingSquare2;
extern int dx4;
extern int dy4;
extern int dx3;
extern int dy3;
extern float ballspeed;
extern float ball2speed;
extern int scorePlayerLeft;
extern int scorePlayerRight;
extern bool quit;
extern bool upPressed;
extern bool downPressed;
extern bool zPressed;
extern bool sPressed;
extern bool ball2spawn;

#endif /* PONG_VARIABLES_H */
