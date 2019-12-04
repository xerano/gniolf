#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


int main(int argc, char** argv){

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    SDL_Surface *surface;
    
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0){
        std::cout << "failed to init sdl" << std::endl;
        return 1;
    }

    if(window == NULL){
        std::cout << "Could not create window: " << SDL_GetError() << std::endl;
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(renderer == NULL){
        std::cout << "Could not create renderer: " << SDL_GetError() << std::endl;
        return 1;
    }

    if(IMG_Init(IMG_INIT_PNG) == 0){
        std::cout << "failed to init image" << IMG_GetError() << std::endl;
        return 1;
    }

    surface = IMG_Load("assets/M484BulletCollection2.png");
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    SDL_Rect srcR, destR;
    srcR.x=19;
    srcR.y=264;
    srcR.h=14;
    srcR.w=10;

    destR.h=srcR.h;
    destR.w=srcR.w;
    destR.x=0;
    destR.y=0;

    uint64_t count = 0;

    // main loop
    while(1) {
        SDL_Event e;
        if(SDL_PollEvent(&e)){
            if(e.type == SDL_QUIT){
                break;
            }
        }

        count++;

        destR.x=destR.x + 1;

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, &srcR, &destR);
        SDL_RenderPresent(renderer);

        SDL_Delay(1000/60);

        std::cout << count << std::endl;
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
}
