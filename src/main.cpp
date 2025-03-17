#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include "Game.h"

constexpr int SCREEN_WIDTH = 1000;
constexpr int SCREEN_HEIGHT = 800;
constexpr int FPS = 60;

int main(int argc, char** args) {
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
	return 0;
}
