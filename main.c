#include "main.h"
#include "collison.h"
#include <stdio.h>
#include <time.h>

SDL_Texture* loadTexture(SDL_Renderer* renderer, const char* path){
    SDL_Surface *surface = IMG_Load(path);
    SDL_Texture *map = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return map;
}

void createScores(GameContext *c) {
    char buffer[8];
    for(int i = 0; i<10; i++){
        SDL_Surface *surface =  TTF_RenderText_Solid(c->message->font, SDL_itoa(i, buffer, 10), c->message->textColor);
        c->scores[i] = SDL_CreateTextureFromSurface(c->renderer, surface);
        SDL_FreeSurface(surface);
    }
}

GameContext* init(){

    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0){
       printf("failed to init sdl");
        return NULL;
    }

    GameContext *context = malloc(sizeof(GameContext));

    context->window = SDL_CreateWindow("GNIOLF <3", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
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

    if( TTF_Init() == -1 )
    {
        printf("failed to init font: %s", TTF_GetError());
        exit(1);    
    }

    context->leftScore = 0;
    context->rightScore = 0;

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
    context->gniolf->speed = 5;
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

    // init paddles
    context->leftPaddle = malloc(sizeof(GameObject));
    context->rightPaddle = malloc(sizeof(GameObject));

    context->leftPaddle->collider.x = 16;
    context->leftPaddle->collider.y = SCREEN_HEIGHT / 2;
    context->leftPaddle->collider.w = 16;
    context->leftPaddle->collider.h = 96;
    context->leftPaddle->srcR.x = 0;
    context->leftPaddle->srcR.y = 0;
    context->leftPaddle->srcR.h = 96;
    context->leftPaddle->srcR.w = 16;

    context->rightPaddle->collider.x = SCREEN_WIDTH - 32;
    context->rightPaddle->collider.y = SCREEN_HEIGHT / 2;
    context->rightPaddle->collider.w = 16;
    context->rightPaddle->collider.h = 96;
    context->rightPaddle->srcR.x = 0;
    context->rightPaddle->srcR.y = 0;
    context->rightPaddle->srcR.h = 96;
    context->rightPaddle->srcR.w = 16;

    context->textureMap = loadTexture(context->renderer, "assets/bullets.png");
    context->textureMapPaddle = loadTexture(context->renderer, "assets/paddle.png");

    SDL_Color textColor = { 255, 255, 255 };
    
    
    context->message = malloc(sizeof(text));
    context->message->textColor = textColor;
    context->message->font = TTF_OpenFont( "assets/joystix.ttf", FONT_SIZE );

    if(context->message->font == NULL) {
        printf("Failed to load font: %s", TTF_GetError());
        exit(1);
    }

    createScores(context);

    return context;
}



int main(int argc, char** argv){

    srand(time(NULL));

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
            SDL_Delay(1000/144);
        }

        SDL_DestroyWindow(context->window);
        SDL_Quit();
        if(context != NULL){
            free(context);
        }
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

    if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
        if(!collision_check(context->topWall, context->rightPaddle->collider)){
            context->rightPaddle->collider.y -= PADDLE_SPEED;

        }
    }

    if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
        if(!collision_check(context->bottomWall, context->rightPaddle->collider)){
            context->rightPaddle->collider.y += PADDLE_SPEED;
        }
    } 
    

    const Uint8 *keyState = SDL_GetKeyboardState(NULL);
    if(keyState[SDL_SCANCODE_UP]){
        if(!collision_check(context->topWall, context->leftPaddle->collider)){
            context->leftPaddle->collider.y -= PADDLE_SPEED;
        }
    }
    if(keyState[SDL_SCANCODE_DOWN]){
        if(!collision_check(context->bottomWall, context->leftPaddle->collider)){
            context->leftPaddle->collider.y += PADDLE_SPEED;
        }
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
    SDL_RenderCopyEx(context->renderer, context->textureMapPaddle, &context->leftPaddle->srcR, &context->leftPaddle->collider, context->leftPaddle->angle, &center, SDL_FLIP_NONE);
    SDL_RenderCopyEx(context->renderer, context->textureMapPaddle, &context->rightPaddle->srcR, &context->rightPaddle->collider, context->rightPaddle->angle, &center, SDL_FLIP_HORIZONTAL);
    
    SDL_Rect textLeftR;
    textLeftR.x = 0;
    textLeftR.y = 0;
    textLeftR.w = 32;
    textLeftR.h = 32;

    SDL_Rect textRightR;
    textRightR.x = SCREEN_WIDTH - 32;
    textRightR.y = 0;
    textRightR.w = 32;
    textRightR.h = 32;

    SDL_RenderCopy(context->renderer, context->scores[context->leftScore], NULL, &textLeftR);
    SDL_RenderCopy(context->renderer, context->scores[context->rightScore], NULL, &textRightR);

    SDL_RenderPresent(context->renderer);
}

void detect_collisions(GameContext *context){
    if(collision_check(context->topWall, context->gniolf->collider)){
        SDL_Log("Collision with top wall...");
        context->gniolf->direction.y *= -1.0;
    }

    if(collision_check(context->bottomWall, context->gniolf->collider)){
        SDL_Log("Collision with bottom wall...");
        context->gniolf->direction.y *= -1.0;
    }

    if(collision_check(context->leftPaddle->collider, context->gniolf->collider)){
        SDL_Log("Collision with left paddle...");
        context->gniolf->direction.x *= -1.0;
        context->gniolf->speed += 1;
    }

    if(collision_check(context->rightPaddle->collider, context->gniolf->collider)){
        SDL_Log("Collision with right paddle...");
        context->gniolf->direction.x *= -1.0;
        context->gniolf->speed += 1;
    }

    if(collision_check(context->leftWall, context->gniolf->collider)){
        SDL_Log("Collision with left wall...");
        context->gniolf->direction.x *= -1.0;
        if(context->rightScore < 9){
            context->rightScore += 1;
        }
    }

    if(collision_check(context->rightWall, context->gniolf->collider)){
        SDL_Log("Collision with right wall...");
        context->gniolf->direction.x *= -1.0;
        if(context->leftScore < 9){
            context->leftScore += 1;
        }
    }


}

void update(GameContext *context){
    context->gniolf->collider.x += context->gniolf->direction.x * context->gniolf->speed;
    context->gniolf->collider.y += context->gniolf->direction.y * context->gniolf->speed;
    context->gniolf->angle += 1.0;
}

