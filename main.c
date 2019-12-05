#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <time.h>

#define SCREEN_HEIGHT 600
#define SCREEN_WIDTH 800

typedef struct Vector {
    float x;
    float y;
} Vector;

typedef struct object{
    int x, y, vx, vy;
    double angle;
    Vector direction;
    SDL_Texture *texture;
    SDL_Rect srcR;
    SDL_Rect collider;
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
    SDL_Rect topWall, bottomWall, leftWall, rightWall;
} GameContext;

void appendObject(GameObjectNode *head, GameObject *o){
    GameObjectNode *current = head;
    while(current->next != NULL){
        current = current->next;
    }
    current->next = malloc(sizeof(GameObjectNode));
    current->next->object = o;
    current->next->next = NULL;
}

// prototypes
int handle_events(GameContext *context);
void detect_collisions(GameContext *context);
int check_collision(SDL_Rect a, SDL_Rect b);
void update(GameContext *context);
void render(GameContext *context);

GameContext* init(){

    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0){
       printf("failed to init sdl");
        return NULL;
    }

    GameContext *context = malloc(sizeof(GameContext));

    context->window = SDL_CreateWindow("GNIOLF", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if(context->window == NULL){
        printf("Could not create window: %s\n", SDL_GetError());
        return NULL;
    }
    
    context->renderer = SDL_CreateRenderer(context->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(context->renderer == NULL){
        printf("Could not create renderer: %s", SDL_GetError());
        return NULL;
    }

    if(IMG_Init(IMG_INIT_PNG) == 0){
            printf("failed to init image: %s", IMG_GetError());
            return NULL;
    }

    SDL_Rect topWall;
    topWall.x = 0;
    topWall.y = 0;
    topWall.w = SCREEN_WIDTH;
    topWall.h = 16;

    SDL_Rect bottomWall;
    bottomWall.x = 0;
    bottomWall.y = SCREEN_HEIGHT - 16;
    bottomWall.w = SCREEN_WIDTH;
    bottomWall.h = 16;

    SDL_Rect leftWall;
    leftWall.x = 0;
    leftWall.y = 0;
    leftWall.w = 16;
    leftWall.h = SCREEN_HEIGHT;

    SDL_Rect rightWall;
    rightWall.x = SCREEN_WIDTH - 16;
    rightWall.y = 0;
    rightWall.w = 16;
    rightWall.h = SCREEN_HEIGHT;

    context->leftWall = leftWall;
    context->rightWall = rightWall;
    context->topWall = topWall;
    context->bottomWall = bottomWall;

    // init the thing
    context->gniolf = malloc(sizeof(GameObject));
    // set initial pos to middle of screen
    context->gniolf->collider.x = SCREEN_WIDTH / 2;
    context->gniolf->collider.y = SCREEN_HEIGHT / 2;
    context->gniolf->collider.w = 32;
    context->gniolf->collider.h = 32;
    context->gniolf->angle=0.0;
    context->gniolf->direction.x = rand() % 1 + ((rand() % 10) / 10.0);
    context->gniolf->direction.y = rand() % 1 + ((rand() % 10) / 10.0);
    context->gniolf->srcR.x = 16;
    context->gniolf->srcR.y = 263;
    context->gniolf->srcR.h = 16;
    context->gniolf->srcR.w = 16;


    SDL_Surface *surface = IMG_Load("assets/bullets.png");
    SDL_Texture *textureMap = SDL_CreateTextureFromSurface(context->renderer, surface);
    SDL_FreeSurface(surface);
    context->textureMap = textureMap;

    return context;
}

int main(int argc, char** argv){

    srand(time(NULL));

    SDL_Texture *texture;
    SDL_Surface *surface;
    
    GameContext *context = init();
    if(context != NULL)
    {   
        int done = 0;
        // main loop
        while(!done) {
            done = handle_events(context);
            detect_collisions(context);
            update(context);
            render(context);
            SDL_Delay(1000/60);
        }

        SDL_DestroyWindow(context->window);
        SDL_Quit();
    }

    
}

int handle_events(GameContext *context){
    int done = 0;
    SDL_Event e;
    while(SDL_PollEvent(&e)){
        switch(e.type) {
            case SDL_WINDOWEVENT_CLOSE:
                if(context->window){
                    SDL_DestroyWindow(context->window);
                    done = 1;
                }
                break;
            case SDL_KEYDOWN:
            {
                switch(e.key.keysym.sym){
                    case SDLK_ESCAPE:
                        done=1;
                    break;
                }
                break;
            }
            case SDL_QUIT:
                done = 1;
                break;
        }
    }

    if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
        SDL_Log("Mouse Button 1 (left) is pressed.");
    }
    
    if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
        SDL_Log("Mouse Button 3 (right) is pressed.");
    }

    const Uint8 *keyState = SDL_GetKeyboardState(NULL);
    if(keyState[SDL_SCANCODE_UP]){
        SDL_Log("UP is pressed.");
    }
    if(keyState[SDL_SCANCODE_DOWN]){
        SDL_Log("DOWN is pressed.");
    }

    return done;
}

void render(GameContext *context) {

    SDL_RenderClear(context->renderer);

    SDL_Rect destR;
    destR.x = context->gniolf->collider.x;
    destR.y = context->gniolf->collider.y;
    destR.h = context->gniolf->srcR.h * 2;
    destR.w = context->gniolf->srcR.w * 2;
    SDL_Point center;
    center.x = 16;
    center.y = 16;
    SDL_RenderCopyEx(context->renderer, context->textureMap, &context->gniolf->srcR, &destR, context->gniolf->angle, &center, SDL_FLIP_NONE);
    
    SDL_RenderPresent(context->renderer);
}

void detect_collisions(GameContext *context){
    if(check_collision(context->topWall, context->gniolf->collider)){
        SDL_Log("Collision with top wall...");
        context->gniolf->direction.y *= -1.0;
    }

    if(check_collision(context->bottomWall, context->gniolf->collider)){
        SDL_Log("Collision with bottom wall...");
        context->gniolf->direction.y *= -1.0;
    }

    if(check_collision(context->leftWall, context->gniolf->collider)){
        SDL_Log("Collision with left wall...");
        context->gniolf->direction.x *= -1.0;
    }

    if(check_collision(context->rightWall, context->gniolf->collider)){
        SDL_Log("Collision with right wall...");
        context->gniolf->direction.x *= -1.0;
    }
}

int check_collision(SDL_Rect a, SDL_Rect b){
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;

    //Calculate the sides of rect A
    leftA = a.x;
    rightA = a.x + a.w;
    topA = a.y;
    bottomA = a.y + a.h;

    //Calculate the sides of rect B
    leftB = b.x;
    rightB = b.x + b.w;
    topB = b.y;
    bottomB = b.y + b.h;

    //If any of the sides from A are outside of B
    if( bottomA <= topB )
    {
        return 0;
    }

    if( topA >= bottomB )
    {
        return 0;
    }

    if( rightA <= leftB )
    {
        return 0;
    }

    if( leftA >= rightB )
    {
        return 0;
    }

    //If none of the sides from A are outside B
    return 1;
}

void update(GameContext *context){
    context->gniolf->collider.x += context->gniolf->direction.x * 5;
    context->gniolf->collider.y += context->gniolf->direction.y * 5;
    context->gniolf->angle += 1.0;
}

