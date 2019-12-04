#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

typedef struct object{
    int x, y;
    SDL_Rect srcR;
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
void render(GameContext *context);

GameContext* init(){
    
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0){
       printf("failed to init sdl");
        return NULL;
    }

    GameContext *context = malloc(sizeof(GameContext));

    context->window = SDL_CreateWindow("GNIOLF", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0);
    if(context->window == NULL){
        printf("Could not create window: %s\n", SDL_GetError());
        return NULL;
    }
    
    context->renderer = SDL_CreateRenderer(context->window, -1, SDL_RENDERER_ACCELERATED);
    if(context->renderer == NULL){
        printf("Could not create renderer: %s", SDL_GetError());
        return NULL;
    }

    if(IMG_Init(IMG_INIT_PNG) == 0){
            printf("failed to init image: %s", IMG_GetError());
            return NULL;
    }

    context->gniolf = malloc(sizeof(GameObject));
    SDL_GetWindowSize(context->window, &context->gniolf->x, &context->gniolf->y);
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

    SDL_Texture *texture;
    SDL_Surface *surface;
    
    GameContext *context = init();
    if(context != NULL)
    {   
        int done = 0;
        // main loop
        while(!done) {
            done = handle_events(context);
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
    destR.x = context->gniolf->x / 2;
    destR.y = context->gniolf->y / 2;
    destR.h = context->gniolf->srcR.h;
    destR.w = context->gniolf->srcR.w;
    SDL_RenderCopy(context->renderer, context->textureMap, &context->gniolf->srcR, &destR);
    SDL_RenderPresent(context->renderer);
}
