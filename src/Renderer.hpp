#pragma once

#include <unordered_map>
#include <string>
#include <fmt/format.h>

extern "C" {
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
}

#include "GameBoard.hpp"

class Renderer {
private:
	void drawGrid(const GameBoard& gameBoard);
	void drawLockedBlocks(const GameBoard& gameBoard);
	void drawTetromino(const Tetromino& tetromino);
	void drawScoreboard(int score, int level);

	shared_ptr<SDL_Renderer> renderer;
	int blockSize;

	unordered_map<TetrominoShape, SDL_Texture*> textures;
public:
	Renderer(shared_ptr<SDL_Renderer> renderer);

	void setRenderer(shared_ptr<SDL_Renderer> renderer);
	void renderBoard(const GameBoard& gb);

	bool loadTexture(const string& filePath, const TetrominoShape shape);
	void renderTexture(const TetrominoShape shape, int x, int y, int width, int height);
	void renderStartScreen( );
	void renderGameOver(int score, int level);
	void renderText(string text, int x, int y, int size, SDL_Color color);
};
