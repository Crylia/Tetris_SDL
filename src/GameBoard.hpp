#pragma once

extern "C" {
#include <SDL2/SDL_mixer.h>
}

#include <vector>
#include <memory>
#include <algorithm>
#include "Tetromino.hpp"
#include "Sound.hpp"

class GameBoard {
private:
	void spawnNewTetromino( );
	bool checkCollision(const Tetromino& tetromino) const;
	void lockTetromino( );
	void clearLines( );

	vector<vector<int>> lockedTetrominos;
	vector<vector<SDL_Color>> lockedColors;
	shared_ptr<Tetromino> currentTetromino;
	shared_ptr<Tetromino> nextTetromino;
	const int width = 10;
	const int height = 18;
	bool collision;
	int score;
	int level;
	int lines;

	const unique_ptr<Sound> sound;

public:
	GameBoard( );
	void update( );
	bool tryMoveCurrentTetromino(int dx, int dy);
	bool tryRotateCurrentTetromino( );
	bool isValidPosition(const vector<vector<int>>& shape, int x, int y) const;
	void moveToBottom( );

	const bool isCollision( ) const;
	const int getScore( ) const;
	const int getLevel( ) const;
	const int getLines( ) const;
	const shared_ptr<Tetromino> getNextTetromino( ) const;

	const vector<vector<int>>& getLockedTetrominos( ) const;
	const vector<vector<SDL_Color>>& getLockedColors( ) const;
	const shared_ptr<Tetromino> getCurrentTetromino( ) const;

	const int getWidth( ) const;
	const int getHeight( ) const;
};
