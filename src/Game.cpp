#include "Game.hpp"

Game::Game( ) :
	window(nullptr, SDL_DestroyWindow),
	gameOver(false),
	startSequence(true),
	quit(false) { }

bool Game::init(const char* title, int w, int h) {
	window.reset(SDL_CreateWindow(
		title,
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		w, h,
		SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE
	));

	if (!window) {
		SDL_Log("Failed to create window: %s", SDL_GetError( ));
		return false;
	}

	renderer = std::shared_ptr<SDL_Renderer>(
		SDL_CreateRenderer(window.get( ), -1, SDL_RENDERER_ACCELERATED),
		[ ](SDL_Renderer* r) { SDL_DestroyRenderer(r); }
	);

	if (!renderer) {
		SDL_Log("Failed to create renderer: %s", SDL_GetError( ));
		return false;
	}

	gameRenderer = make_shared<Renderer>(renderer);
	gameBoard = make_shared<GameBoard>( );

	bgm = std::shared_ptr<Mix_Music>(Mix_LoadMUS("/home/crylia/Dokumente/git/tetris_sdl/assets/bgm.mp3"), [ ](Mix_Music* m) {
		Mix_FreeMusic(m);
		});

	if (!bgm) {
		SDL_Log("Failed to load background music: %s", Mix_GetError( ));
		return false;
	}

	return true;
}

void Game::run( ) {
	while (startSequence) {
		if (quit) return;
		inputHandler( );
		gameRenderer->renderStartScreen( );
	}

	Mix_PlayMusic(bgm.get( ), -1);

	lastUpdateTime = SDL_GetTicks( );
	while (!gameOver && !gameBoard->isCollision( )) {
		if (quit) return;
		inputHandler( );
		update( );
		render( );
	}
	gameOver = true;
	Mix_PauseMusic( );

	while (gameOver) {
		if (quit) return;
		inputHandler( );
		gameRenderer->renderGameOver(gameBoard->getScore( ), gameBoard->getLevel( ));
	}
}

void Game::inputHandler( ) {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) { SDL_Quit( ); } else if (event.type == SDL_KEYDOWN) {
			switch (event.key.keysym.sym) {
			case SDLK_LEFT:
			case SDLK_a:
				gameBoard->tryMoveCurrentTetromino(-1, 0);
				break;
			case SDLK_RIGHT:
			case SDLK_d:
				gameBoard->tryMoveCurrentTetromino(1, 0);
				break;
			case SDLK_DOWN:
			case SDLK_s:
				gameBoard->moveToBottom( );
				break;
			case SDLK_SPACE:
				gameBoard->tryRotateCurrentTetromino( );
				break;
			case SDLK_ESCAPE:
				break;
			case SDLK_g:
				if (startSequence)
					startSequence = false;
				break;
			case SDLK_r:
				if (isGameOver( ))
					restart( );
				break;
			case SDLK_q:
				SDL_Quit( );
				quit = true;
			default:
				break;
			}
		}
	}
}

void Game::update( ) {
	Uint32 currentTime = SDL_GetTicks( );
	Uint32 deltaTime = currentTime - lastUpdateTime;

	if (deltaTime >= max(50, 1000 - (gameBoard->getLevel( ) * 100))) {
		gameBoard->update( );
		lastUpdateTime = currentTime;
	}
}

void Game::render( ) {
	//Set default color to black
	SDL_SetRenderDrawColor(renderer.get( ), 20, 20, 20, 255);
	SDL_RenderClear(renderer.get( ));

	gameRenderer->renderBoard(*gameBoard);

	SDL_RenderPresent(renderer.get( ));
}

bool Game::isGameOver( ) { return gameOver; }
void Game::setGameOver(bool value) { gameOver = value; }

void Game::restart( ) {
	gameOver = false;
	startSequence = true;
	gameBoard = make_shared<GameBoard>( );
}

const bool Game::isGameQuit( )const { return quit; }
