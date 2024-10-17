#pragma once

#include <memory>

extern "C" {
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
}

#include "Renderer.hpp"
#include "GameBoard.hpp"
#include "Sound.hpp"

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

	struct GameState {
		bool gameover = false;
		bool singlePlayer = false;
		bool multiPlayer = false;
		bool startSequence = false;
		bool quit = false;
	} gameState;

	const unique_ptr<Sound> sound;

public:
	Game( );

	bool init(const char* title, int w, int h);
	void run( );
	void restart( );

	const bool isGameOver( ) const;
	const void setGameOver(bool value);

	const bool isGameQuit( ) const;
};
