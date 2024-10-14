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

enum class TetrisAssets {
	BORDER,
	SINGLE,
	I,
	I_MID,
	I_START,
	I_END,
	I_MIDR,
	I_STARTR,
	I_ENDR,
	J,
	L,
	O,
	S,
	Z,
	T,
};

class Renderer {
private:
	void drawGrid(const GameBoard& gameBoard);
	void drawLockedBlocks(const GameBoard& gameBoard);
	void drawTetromino(const Tetromino& tetromino);
	void drawScoreboard(int score, int level);

	TetrisAssets shapeToAsset(const TetrominoShape shape);

	shared_ptr<SDL_Renderer> renderer;

	int blockSize;
	int offsetX;
	int offsetY;
	int screenSpacing = 50;
	int windowHeight = 0;
	int windowWidth = 0;

	unordered_map<TetrisAssets, SDL_Texture*> textures;
public:
	Renderer(shared_ptr<SDL_Renderer> renderer, int w, int h);

	void setRenderer(shared_ptr<SDL_Renderer> renderer);
	void renderBoard(const GameBoard& gb);

	bool loadTexture(const string& filePath, const TetrisAssets shape);
	void renderTexture(const TetrisAssets shape, int x, int y, int width, int height);
	void renderStartScreen( );
	void renderGameOver(int score, int level);
	void renderText(string text, int x, int y, int size, SDL_Color color);

	const int getBlockSize( ) const;
	void setBlockSize(int newBlockSize);
	void setOffset(int newX, int newY);
	const int getOffsetX( ) const;
	const int getOffsetY( ) const;
	void setWindowSize(int w, int h);
};
