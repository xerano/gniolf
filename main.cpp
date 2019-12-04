#include <iostream>
#include <SDL2/SDL.h>


int main(int argc, char** argv){

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    SDL_Surface *surface;
    
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0){
        std::cout << "failed to init sdl" << std::endl;
        return 1;
    }

    window = SDL_CreateWindow("GNIOLF", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0);

    if(window == NULL){
        std::cout << "Could not create window: " << SDL_GetError() << std::endl;
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(renderer == NULL){
        std::cout << "Could not create renderer: " << SDL_GetError() << std::endl;
        return 1;
    }

    // main loop
    while(1) {
        SDL_Event e;
        if(SDL_PollEvent(&e)){
            if(e.type == SDL_QUIT){
                break;
            }
        }
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
    }

    SDL_DestroyWindow(window);

    SDL_Quit();
}
