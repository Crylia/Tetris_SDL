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
	void drawWall(const int w, const int h);
	void drawLockedBlocks(const shared_ptr<GameBoard> gameBoard);
	void drawTetromino(const shared_ptr<Tetromino> tetromino);
	void drawScoreboard(int score, int level, int lines);

	const TetrisAssets shapeToAsset(const TetrominoShape shape) const;

	const shared_ptr<SDL_Renderer> renderer;

	int scale = 5, gridSize = 8;
	int offsetX, offsetY;
	int windowHeight = 0, windowWidth = 0;

	unordered_map<TetrisAssets, string> textures;

	enum class HAlign {
		LEFT,
		CENTER,
		RIGHT,
	};

	enum class VAlign {
		TOP,
		CENTER,
		BOTTOM,
	};

	struct TextDimensions {
		const int x, y, w, h;
		const int fontSize;
	};

	struct TextureDimensions {
		const int x, y, w, h;
		const SDL_Color color;
	};

	TextureDimensions* scoreBoardDimensions;
	TextureDimensions* leftBorder, * rightBorder;

public:
	Renderer(shared_ptr<SDL_Renderer> renderer, int w, int h);

	void renderBoard(const shared_ptr<GameBoard> gameBoard);

	void renderStartScreen( );
	void renderGameOver(shared_ptr<GameBoard> gameBoard);
	TextDimensions renderText(
		const string& text, int x, int y, int fontSize,
		SDL_Color color, HAlign textHAlign = HAlign::LEFT, VAlign textVAlign = VAlign::TOP
	);
	TextureDimensions* renderTexture(
		const string& texturePath, int x, int y, int width = 0, int height = 0,
		SDL_Color color = { 255,255,255 }, float scale = 1.0f, HAlign textHAlign = HAlign::LEFT, VAlign textVAlign = VAlign::TOP
	);
	void renderTetrominoPreview(const shared_ptr<Tetromino> nextTetromino);

	const int getScale( ) const;
	void setScale(int newBlockSize);
	void setOffset(int newX, int newY);
	const int getOffsetX( ) const;
	const int getOffsetY( ) const;
	void setWindowSize(int w, int h);
};
