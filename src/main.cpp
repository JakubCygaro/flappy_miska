#include "Game.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
constexpr int SCREEN_WIDTH = 1000;
constexpr int SCREEN_HEIGHT = 800;
#else
constexpr int SCREEN_WIDTH = 1000;
constexpr int SCREEN_HEIGHT = 800;
#endif
constexpr int FPS = 60;

void game_loop(void)
{
    {
        auto game = Game(SCREEN_WIDTH, SCREEN_HEIGHT, FPS);
        game.initialize();
        game.run();
        game.close();
    }
    TTF_Quit();
    IMG_Quit();
    Mix_Quit();
    SDL_Quit();
}

int main(void)
{
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(game_loop, 0, 1);
#else
    game_loop();
#endif
    return 0;
}
