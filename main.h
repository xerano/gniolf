#ifndef MAIN_H
#define MAIN_H

#define PADDLE_SPEED 5
#define SCREEN_HEIGHT 600
#define SCREEN_WIDTH 800
#define FONT_SIZE 26

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

typedef struct Vector {
    float x;
    float y;
} Vector;

typedef struct text {
    int h,w;
    SDL_Texture *texture;
    TTF_Font *font;
    SDL_Color textColor;
} text;

typedef struct object{
    int x, y, vx, vy;
    double angle;
    Vector direction;
    SDL_Texture *texture;
    SDL_Rect srcR;
    SDL_Rect collider;
    int speed;
} GameObject;

typedef struct node{
    GameObject *object;
    struct node *next;
} GameObjectNode;

typedef struct context {
    SDL_Window *window;
    SDL_Renderer *renderer;
    GameObjectNode *objects;
    GameObject *leftPaddle;
    GameObject *rightPaddle;
    GameObject *gniolf;
    SDL_Texture *textureMap;
    SDL_Texture *textureMapPaddle;
    SDL_Texture *scores[10];
    SDL_Rect topWall, bottomWall, leftWall, rightWall;
    text *message;
    int leftScore;
    int rightScore;
} GameContext;

// prototypes
int handle_events(GameContext *context);
void detect_collisions(GameContext *context);
void update(GameContext *context);
void render(GameContext *context);
void createScores(GameContext *context);
SDL_Texture* loadTexture(SDL_Renderer* renderer, const char* path);

#endif