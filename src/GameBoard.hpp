#pragma once

#include <vector>
#include <memory>
#include <algorithm>
#include "Tetromino.hpp"

class GameBoard {
private:
	void spawnNewTetromino( );
	bool checkCollision(const Tetromino& tetromino) const;
	void lockTetromino( );
	void clearLines( );

	vector<vector<int>> lockedTetrominos;
	vector<vector<SDL_Color>> lockedColors;
	unique_ptr<Tetromino> currentTetromino;
	const int width = 10;
	const int height = 20;
	bool collision;
	int score;
	int level;

public:
	GameBoard( );
	void update( );
	bool tryMoveCurrentTetromino(int dx, int dy);
	void tryRotateCurrentTetromino( );
	bool isValidPosition(const vector<vector<int>>& shape, int x, int y) const;
	void moveToBottom( );
	void reset( );

	const bool isCollision( ) const;
	const int getScore( ) const;
	const int getLevel( ) const;

	const vector<vector<int>>& getLockedTetrominos( ) const;
	const vector<vector<SDL_Color>>& getLockedColors( ) const;
	const Tetromino& getCurrentTetromino( ) const;
};
