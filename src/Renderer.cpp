#include "Renderer.hpp"
#include <iostream>

Renderer::Renderer(shared_ptr<SDL_Renderer> renderer) : renderer(renderer), blockSize(30) {
	setRenderer(renderer);
}

void Renderer::setRenderer(shared_ptr<SDL_Renderer> renderer) {
	this->renderer = renderer;
	loadTexture("/home/crylia/Dokumente/git/tetris_sdl/assets/single.png", TetrominoShape::I);
	loadTexture("/home/crylia/Dokumente/git/tetris_sdl/assets/single.png", TetrominoShape::J);
	loadTexture("/home/crylia/Dokumente/git/tetris_sdl/assets/single.png", TetrominoShape::L);
	loadTexture("/home/crylia/Dokumente/git/tetris_sdl/assets/single.png", TetrominoShape::S);
	loadTexture("/home/crylia/Dokumente/git/tetris_sdl/assets/single.png", TetrominoShape::O);
	loadTexture("/home/crylia/Dokumente/git/tetris_sdl/assets/single.png", TetrominoShape::Z);
}

void Renderer::renderBoard(const GameBoard& gameBoard) {
	drawGrid(gameBoard);
	drawLockedBlocks(gameBoard);
	drawTetromino(gameBoard.getCurrentTetromino( ));
	drawScoreboard(gameBoard.getScore( ), gameBoard.getLevel( ));
}

void Renderer::drawScoreboard(int score, int level) {
	SDL_SetRenderDrawColor(renderer.get( ), 0, 0, 0, 255);
	renderText(fmt::format("Score: {0}", score), 350, 100, 24, SDL_Color{ 200,128,200 });
	renderText(fmt::format("Level: {0}", level), 350, 200, 24, SDL_Color{ 128,200,200 });
}

void Renderer::drawGrid(const GameBoard& board) {
	SDL_SetRenderDrawColor(renderer.get( ), 100, 100, 100, 255);

	int w = board.getLockedTetrominos( )[0].size( ), h = board.getLockedTetrominos( ).size( );
	for (int x = 0; x <= w; ++x)
		SDL_RenderDrawLine(renderer.get( ), x * blockSize, 0, x * blockSize, h * blockSize);

	for (int y = 0; y <= h; ++y)
		SDL_RenderDrawLine(renderer.get( ), 0, y * blockSize, w * blockSize, y * blockSize);
}

void Renderer::drawLockedBlocks(const GameBoard& gameBoard) {
	const auto& lockedTetrominos = gameBoard.getLockedTetrominos( );
	const auto& lockedColors = gameBoard.getLockedColors( );

	for (int row = 0; row < lockedTetrominos.size( ); ++row) {
		for (int col = 0; col < lockedTetrominos[row].size( ); ++col) {
			int blockType = lockedTetrominos[row][col];
			if (blockType != 0) {
				SDL_Rect block = { col * blockSize, row * blockSize, blockSize, blockSize };

				SDL_SetTextureBlendMode(textures[TetrominoShape::I], SDL_BLENDMODE_BLEND);

				SDL_Color color = lockedColors[row][col];
				if (textures[TetrominoShape::I]) {
					SDL_SetTextureColorMod(textures[TetrominoShape::I], color.r, color.g, color.b);
					renderTexture(TetrominoShape::I, block.x, block.y, block.w, block.h);
				} else {
					SDL_SetRenderDrawColor(renderer.get( ), color.r, color.g, color.b, 255);
					SDL_RenderFillRect(renderer.get( ), &block);
				}
			}
		}
	}
}

void Renderer::drawTetromino(const Tetromino& tetromino) {
	const auto& shape = tetromino.getShape( );
	int x = tetromino.getX( ), y = tetromino.getY( );
	auto t = tetromino.getTexture( );

	double angle = tetromino.getRotationAngle( );

	SDL_SetTextureBlendMode(textures[t], SDL_BLENDMODE_BLEND);
	SDL_Color color = tetromino.getColor( );
	SDL_SetTextureColorMod(textures[t], color.r, color.g, color.b);

	for (int row = 0; row < shape.size( ); ++row) {
		for (int col = 0; col < shape[row].size( ); ++col) {
			if (shape[row][col] != 0) {
				SDL_Rect destRect = { (x + col) * blockSize, (y + row) * blockSize, blockSize, blockSize };
				SDL_Point center = { blockSize / 2, blockSize / 2 };
				SDL_RenderCopyEx(renderer.get( ), textures[t], nullptr, &destRect, angle, &center, SDL_FLIP_NONE);
			}
		}
	}
}

bool Renderer::loadTexture(const string& filePath, const TetrominoShape shape) {
	SDL_Texture* texture = IMG_LoadTexture(renderer.get( ), filePath.c_str( ));
	if (!texture) {
		SDL_Log("Failed to load Texture: %s\n%s", filePath, SDL_GetError( ));
		return false;
	}
	textures[shape] = texture;
	return true;
}

void Renderer::renderTexture(const TetrominoShape shape, int x, int y, int width, int height) {
	SDL_Rect destRect{ x,y,width,height };
	SDL_RenderCopy(renderer.get( ), textures[shape], nullptr, &destRect);
}

void Renderer::renderStartScreen( ) {
	SDL_SetRenderDrawColor(renderer.get( ), 0, 0, 0, 255);
	SDL_RenderClear(renderer.get( ));

	renderText("Tetris", 100, 100, 128, SDL_Color{ 200,200,200 });

	SDL_RenderPresent(renderer.get( ));
}

void Renderer::renderGameOver(int score, int level) {
	SDL_SetRenderDrawColor(renderer.get( ), 0, 0, 0, 128);
	SDL_RenderClear(renderer.get( ));

	renderText("Game Over", 100, 100, 128, SDL_Color{ 200, 200, 200 });
	renderText(fmt::format("Score: {0}", score), 100, 250, 64, SDL_Color{ 200, 128, 200 });
	renderText(fmt::format("Level: {0}", level), 100, 350, 64, SDL_Color{ 128, 200, 200 });


	SDL_RenderPresent(renderer.get( ));
}

void Renderer::renderText(string text, int x, int y, int size, SDL_Color color) {
	TTF_Font* font = TTF_OpenFont("/usr/share/fonts/TTF/OpenSans-Regular.ttf", size);
	SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str( ), color);
	if (!surface) {
		SDL_Log("Failed to create surface: %s", TTF_GetError( ));
		return;
	}

	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer.get( ), surface);
	if (!texture) {
		SDL_Log("Failed to create texture from surface: %s", SDL_GetError( ));
		SDL_FreeSurface(surface);
		TTF_CloseFont(font);
		return;
	}

	int width = surface->w;
	int height = surface->h;

	SDL_Rect destRect = { x, y, width, height };

	SDL_RenderCopy(renderer.get( ), texture, nullptr, &destRect);

	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
	TTF_CloseFont(font);
}
