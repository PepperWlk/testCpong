#include <SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

// Structure pour stocker le contexte SDL
typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* backgroundTexture; // Texture pour l'image de fond
    SDL_Texture* texture;
    SDL_Texture* textureball;
    SDL_Surface* image;
} SDL_Context;

// Fonction pour dessiner un carré
int dessin_carre(int x, int y, int width, int height, SDL_Renderer* renderer, SDL_Texture* texture) {
    SDL_Rect rectangle = { x, y, width, height };
    SDL_RenderCopy(renderer, texture, NULL, &rectangle);
}

// Fonction pour vérifier la collision entre deux rectangles
bool checkCollision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2, int* dx3, int* dy3, float* ballspeed, bool* ball2spawn) {
    int left1 = x1;
    int right1 = x1 + w1;
    int top1 = y1;
    int bottom1 = y1 + h1;
    int left2 = x2;
    int right2 = x2 + w2;
    int top2 = y2;
    int bottom2 = y2 + h2;
    if (bottom1 <= top2 || top1 >= bottom2 || right1 <= left2 || left1 >= right2) {
        return false;
    }
    else {
        // Collision detected
        *dx3 = -(*dx3);
        if (rand() % (1 + 1 - 0) + 0 == 1) {
            *dy3 = -(*dy3);
        }
        else {
            *dy3 = *dy3;
        }
        *ballspeed *= 1.005;
        int spawnball = rand() % 101;
        if (spawnball > 51) {
            *ball2spawn = true;
            printf("Nouvelle balle ajoutée !\n");
        }
        return true;
    }
}


// Fonction pour déplacer une barre
void moveBar(bool* upPressed, bool* downPressed, int* yPosition, int speed, int HeightSquare) {
    if (*upPressed) {
        if (*yPosition - speed >= 0) {
            *yPosition -= speed;
        }
    }
    if (*downPressed) {
        if (*yPosition + speed <= 600- HeightSquare) {
            *yPosition += speed;
        }

    }
}

// Fonction pour traiter les événements
void processEvents(bool* quit, bool* upPressed, bool* downPressed, bool* zPressed, bool* sPressed) {
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT) {
            *quit = true;
        }
        else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
            bool isKeyDown = (event.type == SDL_KEYDOWN);
            switch (event.key.keysym.sym) {
            case SDLK_UP:
                *upPressed = isKeyDown;
                break;
            case SDLK_DOWN:
                *downPressed = isKeyDown;
                break;
            case SDLK_z:
                *zPressed = isKeyDown;
                break;
            case SDLK_s:
                *sPressed = isKeyDown;
                break;
            }
        }
    }
}

// Fonction pour initialiser SDL
SDL_Context initializeSDL() {
    SDL_Context context;
    context.window = NULL;
    context.renderer = NULL;
    context.backgroundTexture = NULL; // Initialisation de la texture de fond à NULL

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Failed to load SDL\n");
        return context;
    }
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        // Handle error
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Erreur lors de l'initialisation de SDL_mixer : %s\n", Mix_GetError());
        return context;
    }
    Mix_Music* sound = Mix_LoadMUS("C:/Users/lguillemart/source/repos/Project1/Project1/musique.mp3");
    if (sound == NULL) {
        // Handle error
    }

    context.window = SDL_CreateWindow("Pong Game",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        800, 600, SDL_WINDOW_SHOWN);
    if (context.window == NULL) {
        printf("Failed to create window\n");
        SDL_Quit();
        return context;
    }

    context.renderer = SDL_CreateRenderer(context.window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (context.renderer == NULL) {
        printf("Failed to create renderer\n");
        SDL_DestroyWindow(context.window);
        SDL_Quit();
        return context;
    }

    // Chargement de l'image de fond
    context.backgroundTexture = IMG_LoadTexture(context.renderer, "8888.bmp");
    if (context.backgroundTexture == NULL) {
        printf("Failed to load background image: %s\n", IMG_GetError());
        SDL_DestroyRenderer(context.renderer);
        SDL_DestroyWindow(context.window);
        SDL_Quit();
        return context;
    }

    context.texture = IMG_LoadTexture(context.renderer, "1111.jpg");
    if (context.texture == NULL) {
        printf("Failed to load image: %s\n", IMG_GetError());
        SDL_DestroyRenderer(context.renderer);
        SDL_DestroyWindow(context.window);
        SDL_Quit();
        return context;
    }


    context.textureball = IMG_LoadTexture(context.renderer, "image.png");
    if (context.textureball == NULL) {
        printf("Failed to load image ball: %s\n", IMG_GetError());
        SDL_DestroyRenderer(context.renderer);
        SDL_DestroyWindow(context.window);
        SDL_Quit();
        return context;
    }

    return context;
}

void drawScores(SDL_Renderer* renderer, int scoreLeft, int scoreRight, int startX, int startY, int width, int height, int gap) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White

    // Draw score for the left player
    for (int i = 0; i < scoreLeft; i++) {
        SDL_Rect rect = { startX + i * (width + gap), startY, width, height };
        SDL_RenderFillRect(renderer, &rect);
    }

    // Draw score for the right player
    int startXRight = 800 - startX - (scoreRight * (width + gap)); // Adjust start position for right player
    for (int i = 0; i < scoreRight; i++) {
        SDL_Rect rect = { startXRight + i * (width + gap), startY, width, height };
        SDL_RenderFillRect(renderer, &rect);
    }
}

void renderBallSpawnEffect(SDL_Renderer* renderer, int x, int y, int frame) {
    int maxSize = 30; // Maximum size of the ball for the effect
    int normalSize = 10; // Normal size of the ball
    SDL_Color effectColor = { 255, 255, 255, 255 }; // Color for the spawn effect (white)

    // Calculate the size based on the current frame of the effect
    int size = maxSize - ((maxSize - normalSize) * frame / 10);
    if (size < normalSize) size = normalSize;

    // Set the render color for the ball effect
    SDL_SetRenderDrawColor(renderer, effectColor.r, effectColor.g, effectColor.b, effectColor.a);

    // Draw the ball with the current effect size
    SDL_Rect ballRect = { x - size / 2, y - size / 2, size, size };
    SDL_RenderFillRect(renderer, &ballRect);
}


void verifgoal(int* xBall, int* scorePlayerRight, int* yBall, int SizeMovingSquare, int* scorePlayerLeft, int* dy3, int frame, SDL_Renderer* renderer)
{
    if (*xBall <= 0) {
        (*scorePlayerRight)++;
        *xBall = 400; // Reset ball position to the center 
        *yBall = 300;
        renderBallSpawnEffect(renderer, xBall, yBall, frame);
    }

    if (*xBall + SizeMovingSquare >= 800) {
        (*scorePlayerLeft)++;
        *xBall = 400; // Reset ball position to the center  
        *yBall = 300;
        renderBallSpawnEffect(renderer, xBall, yBall, frame);
    }

    if (*yBall <= 0 || *yBall + SizeMovingSquare >= 600) {
        *dy3 = -(*dy3);
    }
}

void moveSecondBall(bool* ball2spawn, int* xBall2, int* yBall2, int* dx4, int* dy4, float* ball2speed, int SizeMovingSquare, int PosxSquareRight, int PosySquareRight, int PosxSquareLeft,
    int PosySquareLeft, int WidthSquare, int HeightSquare, SDL_Renderer* renderer, SDL_Texture* textureball, int* dx3, int* dy3, float* ballspeed, bool* ball1spawn, int* scorePlayerRight, int* scorePlayerLeft) {
    if (*ball2spawn) {
        dessin_carre(*xBall2, *yBall2, SizeMovingSquare, SizeMovingSquare, renderer, textureball);

        *xBall2 += *dx4 * (*ball2speed);
        *yBall2 += *dy4 * (*ball2speed);
        if (*xBall2 <= 0 || *xBall2 + SizeMovingSquare >= 800) {
            *dx4 = -(*dx4);
        }
        if (*yBall2 <= 0 || *yBall2 + SizeMovingSquare >= 600) {
            *dy4 = -(*dy4);
        }

        bool collision3 = checkCollision(*xBall2, *yBall2, SizeMovingSquare, SizeMovingSquare,
            PosxSquareRight, PosySquareRight, WidthSquare, HeightSquare, dx4, dy4, ball2speed, ball1spawn);

        bool collision4 = checkCollision(*xBall2, *yBall2, SizeMovingSquare, SizeMovingSquare,
            PosxSquareLeft, PosySquareLeft, WidthSquare, HeightSquare, dx4, dy4, ball2speed, ball1spawn);

        // Vérifier les buts pour la balle 2
        if (*xBall2 <= 0) {
            (*scorePlayerRight)++;
            *xBall2 = 400; // Reset ball position to the center 
            *yBall2 = 300;
            *dy4 = -(*dy4); // Inverser la direction verticale
        }

        if (*xBall2 + SizeMovingSquare >= 800) {
            (*scorePlayerLeft)++;
            *xBall2 = 400; // Reset ball position to the center  
            *yBall2 = 300;
            *dy4 = -(*dy4); // Inverser la direction verticale
        }
    }
}

void updateBallPosition(int* xBall, int* yBall, int SizeMovingSquare, int* scorePlayerRight, int* scorePlayerLeft, int* dy3, int* dx3, float* ballspeed, SDL_Renderer* renderer, SDL_Texture* textureball) {
    // Mise à jour de la position de la balle
    *xBall += *dx3 * (*ballspeed); 
    *yBall += *dy3 * (*ballspeed); 

    Mix_Music* sound = Mix_LoadMUS("C:/Users/lguillemart/source/repos/Project1/Project1/musique.mp3");
    if (sound == NULL) {
        // Handle error
    }

    //audio spwn nouvelle ballle
    Mix_PlayMusic(sound, -1);

    // Dessiner la balle
    dessin_carre(*xBall, *yBall, SizeMovingSquare, SizeMovingSquare, renderer, textureball); 

    // Vérifier les buts
    verifgoal(xBall, scorePlayerRight, yBall, SizeMovingSquare, scorePlayerLeft, dy3, 2, renderer);

    // Afficher les scores
    drawScores(renderer, *scorePlayerLeft, *scorePlayerRight, 50, 10, 20, 10, 5); 

    // Rafraîchir l'affichage
    SDL_RenderPresent(renderer); 
    SDL_Delay(10); 
}

void cleanUpSDL(SDL_Context context) {
    SDL_DestroyTexture(context.backgroundTexture);
    SDL_DestroyTexture(context.texture); 
    SDL_DestroyTexture(context.textureball); 
    SDL_DestroyRenderer(context.renderer); 
    SDL_DestroyWindow(context.window); 
    SDL_Quit(); 
}

SDL_Context initializeAndSeedSDL() {
    // Initialiser la graine pour la génération de nombres aléatoires
    srand(time(NULL));

    // Initialiser SDL
    SDL_Context context = initializeSDL();

    // Vérifier si l'initialisation a échoué
    if (context.window == NULL || context.renderer == NULL) {
        // Gérer l'erreur ici
        context.window = NULL; // Pour éviter que la fenêtre ne soit détruite par la fonction cleanUpSDL
    }

    return context;
}

void handlePlayerEventsAndMovement(bool* quit, bool* upPressed, bool* downPressed, bool* zPressed, bool* sPressed, int* PosySquareRight, int* PosySquareLeft, int HeightSquare) {
    processEvents(quit, upPressed, downPressed, zPressed, sPressed);
    moveBar(upPressed, downPressed, PosySquareRight, 10, HeightSquare);
    moveBar(zPressed, sPressed, PosySquareLeft, 10, HeightSquare);
}


void tout_carres(int PosxSquareRight, int PosySquareRight, int PosxSquareLeft, int PosySquareLeft, int WidthSquare, int HeightSquare, SDL_Renderer* renderer, SDL_Texture* texture) {
    // Dessiner le carré de droite
    dessin_carre(PosxSquareRight, PosySquareRight, WidthSquare, HeightSquare, renderer, texture);

    // Dessiner le carré de gauche
    dessin_carre(PosxSquareLeft, PosySquareLeft, WidthSquare, HeightSquare, renderer, texture);
}

int main(int argc, char* argv[]) {
    SDL_Context context = initializeAndSeedSDL();

    // Variables de jeu
    int PosxSquareRight = 750, PosxSquareLeft = 50, PosySquareRight = 200, PosySquareLeft = 200;
    int WidthSquare = 20, HeightSquare = 150;
    int xBall = 400, yBall = 300, SizeMovingSquare = 15;
    int xBall2 = 400, yBall2 = 300;
    int dx4 = (rand() % 2 == 0) ? -5 : 5, dy4 = (rand() % 2 == 0) ? -5 : 5, dx3 = (rand() % 2 == 0) ? -5 : 5, dy3 = (rand() % 2 == 0) ? -5 : 5;
    float ballspeed = 1, ball2speed = 1;
    int scorePlayerLeft = 0;
    int scorePlayerRight = 0;
    bool quit = false, upPressed = false, downPressed = false, zPressed = false, sPressed = false, ball2spawn = false;

    while (!quit) {
        handlePlayerEventsAndMovement(&quit, &upPressed, &downPressed, &zPressed, &sPressed, &PosySquareRight, &PosySquareLeft, HeightSquare);

        SDL_SetRenderDrawColor(context.renderer, 0, 0, 0, 255);
        SDL_RenderClear(context.renderer);

        // Dessiner l'image de fond
        SDL_RenderCopy(context.renderer, context.backgroundTexture, NULL, NULL);

        // Dessiner les carrés
        tout_carres(PosxSquareRight, PosySquareRight, PosxSquareLeft, PosySquareLeft, WidthSquare, HeightSquare, context.renderer, context.texture);
        
        // Vérifier les collisions  
        bool collision1 = checkCollision(xBall, yBall, SizeMovingSquare, SizeMovingSquare, 
            PosxSquareRight, PosySquareRight, WidthSquare, HeightSquare, &dx3, &dy3, &ballspeed, &ball2spawn); 

        bool collision2 = checkCollision(xBall, yBall, SizeMovingSquare, SizeMovingSquare, 
            PosxSquareLeft, PosySquareLeft, WidthSquare, HeightSquare, &dx3, &dy3, &ballspeed, &ball2spawn); 


        moveSecondBall(&ball2spawn, &xBall2, &yBall2, &dx4, &dy4, &ball2speed, SizeMovingSquare,
            PosxSquareRight, PosySquareRight, PosxSquareLeft, PosySquareLeft, WidthSquare, HeightSquare,
            context.renderer, context.textureball, &dx3, &dy3, &ballspeed, &ball2spawn, &scorePlayerRight, &scorePlayerLeft);


        updateBallPosition(&xBall, &yBall, SizeMovingSquare, &scorePlayerRight, &scorePlayerLeft, &dy3, &dx3, &ballspeed, context.renderer, context.textureball);
    }

    // Appel de la fonction cleanUpSDL
    cleanUpSDL(context);
    return 0;
}


