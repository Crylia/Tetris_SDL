#include "Game.hpp"

Game::Game( ) : window(nullptr, SDL_DestroyWindow), sound(make_unique<Sound>( )) { }

bool Game::init(const char* title, int w, int h) {
	window.reset(SDL_CreateWindow(
		title,
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		w, h,
		SDL_WINDOW_SHOWN
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
	int ww, wh;
	SDL_GetWindowSize(window.get( ), &ww, &wh);

	gameState.startSequence = true;

	gameRenderer = make_shared<Renderer>(renderer, ww, wh);
	gameBoard = make_shared<GameBoard>( );

	handleWindowResize( );

	return true;
}

void Game::run( ) {
	while (gameState.startSequence) {
		if (gameState.quit) return;
		inputHandler( );
		gameRenderer->renderStartScreen( );
	}

	sound->PlayMusic(MusicName::MAIN_THEME);
	lastUpdateTime = SDL_GetTicks( );
	while (!gameState.gameover && !gameBoard->isCollision( )) {
		if (gameState.quit) return;
		inputHandler( );
		update( );
		render( );
	}

	gameState.gameover = true;
	sound->PauseMusic( );
	sound->PlaySound(SoundName::GAME_OVER);
	while (gameState.gameover) {
		if (gameState.quit) return;
		SDL_SetRenderDrawColor(renderer.get( ), 248, 248, 248, 255);
		SDL_RenderClear(renderer.get( ));
		inputHandler( );
		gameRenderer->renderGameOver(gameBoard);
	}
}

void Game::inputHandler( ) {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			SDL_Quit( );
			gameState.quit = true;
		} else if (event.type == SDL_KEYDOWN) {
			switch (event.key.keysym.sym) {
			case SDLK_LEFT:
			case SDLK_a:
				if (!gameState.gameover && !gameState.startSequence)
					if (gameBoard->tryMoveCurrentTetromino(-1, 0))
						sound->PlaySound(SoundName::MOVE_PIECE);
				break;
			case SDLK_RIGHT:
			case SDLK_d:
				if (!gameState.gameover && !gameState.startSequence)
					if (gameBoard->tryMoveCurrentTetromino(1, 0))
						sound->PlaySound(SoundName::MOVE_PIECE);
				break;
			case SDLK_DOWN:
			case SDLK_s:
				if (!gameState.gameover && !gameState.startSequence) {
					gameBoard->moveToBottom( );
					sound->PlaySound(SoundName::PIECE_LANDED);
				}
				break;
			case SDLK_SPACE:
				if (!gameState.gameover && !gameState.startSequence)
					if (gameBoard->tryRotateCurrentTetromino( ))
						sound->PlaySound(SoundName::ROTATE_PIECE);
				break;
			case SDLK_ESCAPE:
				break;
			case SDLK_g:
				if (gameState.startSequence) {
					gameState.startSequence = false;
					sound->PlaySound(SoundName::MENU);
				}
				break;
			case SDLK_r:
				if (isGameOver( ))
					restart( );
				break;
			case SDLK_q:
				SDL_Quit( );
				gameState.quit = true;
			case SDLK_EQUALS:
				SDL_Log("Test %d", Mix_GetMusicVolume(bgm.get( )));
				Mix_VolumeMusic(Mix_GetMusicVolume(bgm.get( )) + 8);
				break;
			case SDLK_MINUS:
				SDL_Log("Test %d", Mix_GetMusicVolume(bgm.get( )));

				Mix_VolumeMusic(Mix_GetMusicVolume(bgm.get( )) - 8);
				break;
			case SDLK_m:
				sound->IsMusicPlaying( ) ? sound->PauseMusic( ) : sound->ResumeMusic( );
				break;
			default:
				break;
			}
		} else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
			int TARGET_ASPECT_RATIO = 3 / 4;
			int newHeight = event.window.data2, newWidth = event.window.data1;

			float newAspectRatio = static_cast<float>(newWidth) / newHeight;

			if (newAspectRatio > TARGET_ASPECT_RATIO)
				newWidth = static_cast<int>(newHeight * TARGET_ASPECT_RATIO);
			else
				newHeight = static_cast<int>(newWidth / TARGET_ASPECT_RATIO);

			SDL_SetWindowSize(window.get( ), newWidth, newHeight);

			handleWindowResize( );
		}
	}
}

void Game::handleWindowResize( ) {
	int windowWidth, windowHeight;
	SDL_GetWindowSize(window.get( ), &windowWidth, &windowHeight);

	//gameRenderer->setScale(windowHeight / gameBoard->getHeight( ));


	gameRenderer->setWindowSize(windowWidth, windowHeight);
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
	// Background color
	SDL_SetRenderDrawColor(renderer.get( ), 248, 248, 248, 255);
	SDL_RenderClear(renderer.get( ));

	gameRenderer->renderBoard(gameBoard);
	gameRenderer->renderTetrominoPreview(gameBoard->getNextTetromino( ));

	SDL_RenderPresent(renderer.get( ));
}

void Game::restart( ) {
	gameState.gameover = false;
	gameState.startSequence = true;
	gameBoard = make_shared<GameBoard>( );
}

const bool Game::isGameOver( ) const { return gameState.gameover; }
const void Game::setGameOver(bool value) { gameState.gameover = value; }
const bool Game::isGameQuit( )const { return gameState.quit; }
