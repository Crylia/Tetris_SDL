#pragma once

#include <memory>

extern "C" {
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
}

#include "Renderer.hpp"
#include "GameBoard.hpp"

using namespace std;

class Game {
private:
	void update( );
	void render( );
	void inputHandler( );

	void handleWindowResize( );

	unique_ptr<SDL_Window, void(*)(SDL_Window*)> window;
	shared_ptr<SDL_Renderer> renderer;

	shared_ptr<GameBoard> gameBoard;
	shared_ptr<Renderer> gameRenderer;

	shared_ptr<Mix_Music> bgm;

	Uint32 lastUpdateTime = 0;
	int dropInterval = 1000;

	bool gameOver;
	bool startSequence;
	bool quit;

public:
	Game( );

	bool init(const char* title, int w, int h);
	void run( );
	void restart( );
	bool isGameOver( );
	void setGameOver(bool value);

	const bool isGameQuit( ) const;
};
