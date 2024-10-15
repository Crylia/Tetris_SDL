#include <iostream>

extern "C" {
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
}

#include "Game.hpp"

int main( ) {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
		SDL_Log("Couldn't init SDL: %s", SDL_GetError( ));
		return 1;
	}

	if (IMG_Init(IMG_INIT_PNG) == 0) {
		std::cerr << "Failed to initialize SDL_image: " << IMG_GetError( ) << std::endl;
		SDL_Quit( );
		return 1;
	}

	if (TTF_Init( ) == -1) {
		std::cerr << "Failed to initialize SDL_ttf: " << TTF_GetError( ) << std::endl;
		SDL_Quit( );
		return 1;
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
		SDL_Log("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError( ));
		return 1;
	}

	Game game;
	if (!game.init("Tetris", 810, 600)) {
		SDL_Log("Failed to init game");
		SDL_Quit( );
		return 1;
	}

	while (!game.isGameQuit( ))
		game.run( );

	SDL_Quit( );

	return 0;
}
