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
		SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN
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

	gameRenderer = make_shared<Renderer>(renderer, ww, wh);
	gameBoard = make_shared<GameBoard>( );

	handleWindowResize( );

	bgm = std::shared_ptr<Mix_Music>(Mix_LoadMUS("assets/bgm.mp3"), [ ](Mix_Music* m) {
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

	Mix_Chunk* rotateSound = Mix_LoadWAV("assets/sound_effects/game_over.wav");
	if (rotateSound == nullptr)
		SDL_Log("Failed to play rotate sound effect: %s", Mix_GetError( ));
	else {
		Mix_PlayChannel(-1, rotateSound, 0);
	}

	while (gameOver) {
		if (quit) return;
		inputHandler( );
		render( );
		gameRenderer->renderGameOver(gameBoard->getScore( ), gameBoard->getLevel( ), gameBoard->getLines( ));
	}
}

void Game::inputHandler( ) {
	SDL_Event event;
	Mix_Chunk* movePieceSound = Mix_LoadWAV("assets/sound_effects/move_piece.wav");
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			SDL_Quit( );
			quit = true;
		} else if (event.type == SDL_KEYDOWN) {
			switch (event.key.keysym.sym) {
			case SDLK_LEFT:
			case SDLK_a:
				gameBoard->tryMoveCurrentTetromino(-1, 0);
				if (movePieceSound == nullptr)
					SDL_Log("Failed to play rotate sound effect: %s", Mix_GetError( ));
				else {
					Mix_PlayChannel(-1, movePieceSound, 0);
				}
				break;
			case SDLK_RIGHT:
			case SDLK_d:
				gameBoard->tryMoveCurrentTetromino(1, 0);
				if (movePieceSound == nullptr)
					SDL_Log("Failed to play rotate sound effect: %s", Mix_GetError( ));
				else {
					Mix_PlayChannel(-1, movePieceSound, 0);
				}
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
				if (startSequence) {
					startSequence = false;
					Mix_Chunk* menuSound = Mix_LoadWAV("assets/sound_effects/menu.wav");
					if (menuSound == nullptr)
						SDL_Log("Failed to play rotate sound effect: %s", Mix_GetError( ));
					else {
						Mix_PlayChannel(-1, menuSound, 0);
					}
				}
				break;
			case SDLK_r:
				if (isGameOver( ))
					restart( );
				break;
			case SDLK_q:
				SDL_Quit( );
				quit = true;
			case SDLK_EQUALS:
				SDL_Log("Test %d", Mix_GetMusicVolume(bgm.get( )));
				Mix_VolumeMusic(Mix_GetMusicVolume(bgm.get( )) + 8);
				break;
			case SDLK_MINUS:
				SDL_Log("Test %d", Mix_GetMusicVolume(bgm.get( )));

				Mix_VolumeMusic(Mix_GetMusicVolume(bgm.get( )) - 8);
				break;
			case SDLK_m:
				if (Mix_PausedMusic( ))
					Mix_ResumeMusic( );
				else
					Mix_PauseMusic( );

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

	gameRenderer->setBlockSize(windowHeight / gameBoard->getLockedTetrominos( ).size( ));

	int offsetX = (windowWidth - gameBoard->getLockedTetrominos( )[0].size( ) * gameRenderer->getBlockSize( )) / 2;
	int offsetY = (windowHeight - gameBoard->getLockedTetrominos( ).size( ) * gameRenderer->getBlockSize( )) / 2;
	gameRenderer->setOffset(offsetX, offsetY);

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
	//Set default color to black
	SDL_SetRenderDrawColor(renderer.get( ), 248, 248, 248, 255);
	SDL_RenderClear(renderer.get( ));

	gameRenderer->renderBoard(*gameBoard);
	gameRenderer->renderTetrominoPreview(gameBoard->getNextTetromino( ));

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
