#include "GameBoard.hpp"
#include <iostream>

GameBoard::GameBoard( )
	: lockedTetrominos(20, vector<int>(10, 0)), lockedColors(20, std::vector<SDL_Color>(10, { 0, 0, 0, 0 })), score(0), level(1), collision(false) {
	spawnNewTetromino( );
}

bool GameBoard::tryMoveCurrentTetromino(int dx, int dy) {
	currentTetromino->move(dx, dy);
	if (checkCollision(*currentTetromino)) {
		currentTetromino->move(-dx, -dy);
		return false;
	}
	return true;
}

void GameBoard::tryRotateCurrentTetromino( ) {
	currentTetromino->rotate(*this);
	if (checkCollision(*currentTetromino))
		for (int i = 0; i < 3; i++)
			currentTetromino->rotate(*this);
}

bool GameBoard::checkCollision(const Tetromino& tetromino) const {
	const auto& shape = tetromino.getShape( );
	int x = tetromino.getX( ), y = tetromino.getY( );

	for (int row = 0; row < shape.size( ); ++row)
		for (int col = 0; col < shape[row].size( ); ++col)
			if (shape[row][col] != 0) {
				int newX = x + col;
				int newY = y + row;

				if (newX < 0 || newX >= width || newY >= height || (newY >= 0 && lockedTetrominos[newY][newX] != 0)) { return true; }
			}

	return false;
}

void GameBoard::lockTetromino( ) {
	const auto& shape = currentTetromino->getShape( );
	int x = currentTetromino->getX( ), y = currentTetromino->getY( );

	for (int row = 0; row < shape.size( ); ++row)
		for (int col = 0; col < shape[row].size( ); ++col)
			if (shape[row][col] != 0) {
				int lockedTetrominosX = x + col;
				int lockedTetrominosY = y + row;
				if (lockedTetrominosY >= 0 && lockedTetrominosX < height) {
					lockedTetrominos[lockedTetrominosY][lockedTetrominosX] = 1;
					lockedColors[lockedTetrominosY][lockedTetrominosX] = currentTetromino->getColor( );
				}
			}
}

void GameBoard::clearLines( ) {
	for (int row = 0; row < height; row++)
		if (all_of(lockedTetrominos[row].begin( ), lockedTetrominos[row].end( ), [ ](int cell) {return cell != 0;})) {
			lockedTetrominos.erase(lockedTetrominos.begin( ) + row);
			lockedTetrominos.insert(lockedTetrominos.begin( ), vector<int>(width, 0));
			score += 100;
			if ((score % 1000) == 0)
				level++;
		}
}

void GameBoard::spawnNewTetromino( ) {
	random_device dev;
	mt19937 rng(dev( ));
	uniform_int_distribution<mt19937::result_type> dist6(0, static_cast<int>(TetrominoShape::COUNT) - 1);
	TetrominoShape shape = static_cast<TetrominoShape>(dist6(rng));
	currentTetromino = make_unique<Tetromino>(shape);
	currentTetromino->setTexture(shape);
	currentTetromino->move(width / 2 - 1, 0);

	if (checkCollision(*currentTetromino))
		collision = true;
}

void GameBoard::update( ) {
	if (!tryMoveCurrentTetromino(0, 1)) {
		lockTetromino( );
		clearLines( );
		spawnNewTetromino( );
	} else
		clearLines( );
}

bool GameBoard::isValidPosition(const vector<vector<int>>& shape, int x, int y) const {
	for (int row = 0; row < shape.size( ); ++row)
		for (int col = 0; col < shape[row].size( ); ++col)
			if (shape[row][col]) {
				int newX = x + col;
				int newY = y + row;

				if (newX < 0 || newX >= width || newY < 0 || newY >= height) return false;
				if (lockedTetrominos[newY][newX]) return false;
			}
	return true;
}

void GameBoard::moveToBottom( ) {
	while (isValidPosition(currentTetromino->getShape( ), currentTetromino->getX( ), currentTetromino->getY( ) + 1)) currentTetromino->move(0, 1);
}

const bool GameBoard::isCollision( ) const { return collision; }
const int GameBoard::getScore( ) const { return score; }
const int GameBoard::getLevel( ) const { return level; }

const vector<vector<int>>& GameBoard::getLockedTetrominos( ) const { return lockedTetrominos; }
const vector<vector<SDL_Color>>& GameBoard::getLockedColors( ) const { return lockedColors; }
const Tetromino& GameBoard::getCurrentTetromino( ) const { return *currentTetromino; }

void GameBoard::reset( ) {
	lockedTetrominos.clear( );
	lockedColors.clear( );
	score = 0;
	level = 1;
}
