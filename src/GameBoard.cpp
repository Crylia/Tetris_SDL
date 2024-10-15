#include "GameBoard.hpp"
#include <iostream>

GameBoard::GameBoard( )
	: lockedTetrominos(20, vector<int>(10, 0)), lockedColors(20, std::vector<SDL_Color>(10, { 0, 0, 0, 255 })), score(0), level(0), lines(0), collision(false) {
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
	else {
		Mix_Chunk* rotateSound = Mix_LoadWAV("assets/sound_effects/rotate_piece.wav");
		if (rotateSound == nullptr)
			SDL_Log("Failed to play rotate sound effect: %s", Mix_GetError( ));
		else {
			Mix_PlayChannel(-1, rotateSound, 0);
		}
	}
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
	double angle = currentTetromino->getRotationAngle( );
	TetrominoShape tetrominoShape = currentTetromino->getShapeEnumn( );

	if (tetrominoShape == TetrominoShape::I) {
		if (angle == 90 || angle == 270) {
			for (int col = 0; col < shape[0].size( ); ++col) {
				int lockedTetrominosX = x + col;
				int lockedTetrominosY = y;

				if (lockedTetrominosY >= 0 && lockedTetrominosX >= 0 && lockedTetrominosX < width) {
					if (col == 0) {
						lockedTetrominos[lockedTetrominosY][lockedTetrominosX] = static_cast<int>(TetrominoShape::I_ENDR) + 1;
					} else if (col == 3) {
						lockedTetrominos[lockedTetrominosY][lockedTetrominosX] = static_cast<int>(TetrominoShape::I_STARTR) + 1;
					} else {
						lockedTetrominos[lockedTetrominosY][lockedTetrominosX] = static_cast<int>(TetrominoShape::I_MIDR) + 1;
					}
					lockedColors[lockedTetrominosY][lockedTetrominosX] = currentTetromino->getColor( );
				}
			}
		} else {
			for (int row = 0; row < shape.size( ); ++row) {
				for (int col = 0; col < shape[row].size( ); ++col) {
					if (shape[row][col] != 0) {
						int lockedTetrominosX = x;
						int lockedTetrominosY = y + row;

						if (lockedTetrominosY >= 0 && lockedTetrominosX >= 0 && lockedTetrominosX < width && lockedTetrominosY < height) {
							if (row == 0) {
								lockedTetrominos[lockedTetrominosY][lockedTetrominosX] = static_cast<int>(TetrominoShape::I_END) + 1;
							} else if (row == 3) {
								lockedTetrominos[lockedTetrominosY][lockedTetrominosX] = static_cast<int>(TetrominoShape::I_START) + 1;
							} else {
								lockedTetrominos[lockedTetrominosY][lockedTetrominosX] = static_cast<int>(TetrominoShape::I_MID) + 1;
							}
							lockedColors[lockedTetrominosY][lockedTetrominosX] = currentTetromino->getColor( );
						}
					}
				}
			}
		}
	} else {
		for (int row = 0; row < shape.size( ); ++row) {
			for (int col = 0; col < shape[row].size( ); ++col) {
				if (shape[row][col] != 0) {
					int lockedTetrominosX = x + col;
					int lockedTetrominosY = y + row;

					if (lockedTetrominosY >= 0 && lockedTetrominosX >= 0 && lockedTetrominosX < width && lockedTetrominosY < height) {
						lockedTetrominos[lockedTetrominosY][lockedTetrominosX] = static_cast<int>(tetrominoShape) + 1;
						lockedColors[lockedTetrominosY][lockedTetrominosX] = currentTetromino->getColor( );
					}
				}
			}
		}
	}

	Mix_Chunk* pieceLanded = Mix_LoadWAV("assets/sound_effects/piece_landed.wav");
	if (pieceLanded == nullptr)
		SDL_Log("Failed to play sound effect: %s", Mix_GetError( ));
	else {
		Mix_PlayChannel(-1, pieceLanded, 0);
	}
}

void GameBoard::clearLines( ) {
	int clearedLines = 0;
	for (int row = 0; row < height; row++) {
		if (all_of(lockedTetrominos[row].begin( ), lockedTetrominos[row].end( ), [ ](int cell) { return cell != 0; })) {
			lockedTetrominos.erase(lockedTetrominos.begin( ) + row);
			lockedColors.erase(lockedColors.begin( ) + row);

			lockedTetrominos.insert(lockedTetrominos.begin( ), vector<int>(width, 0));
			lockedColors.insert(lockedColors.begin( ), vector<SDL_Color>(width, { 0, 0, 0, 0 }));

			score += 100;
			if (score % 1000 == 0) {
				level++;
				Mix_Chunk* levelUpSound = Mix_LoadWAV("assets/sound_effects/level_up.wav");
				if (levelUpSound == nullptr)
					SDL_Log("Failed to play rotate sound effect: %s", Mix_GetError( ));
				else {
					Mix_PlayChannel(-1, levelUpSound, 0);
				}
			}
			clearedLines++;
			lines++;
		}
	}

	if (clearedLines >= 4) {
		Mix_Chunk* clearedLinesSound = Mix_LoadWAV("assets/sound_effects/tetris_line_clear.wav");
		if (clearedLinesSound == nullptr)
			SDL_Log("Failed to play rotate sound effect: %s", Mix_GetError( ));
		else {
			Mix_PlayChannel(-1, clearedLinesSound, 0);
		}
	} else if (clearedLines > 0) {
		Mix_Chunk* clearedLinesSound = Mix_LoadWAV("assets/sound_effects/line_clear.wav");
		if (clearedLinesSound == nullptr)
			SDL_Log("Failed to play rotate sound effect: %s", Mix_GetError( ));
		else {
			Mix_PlayChannel(-1, clearedLinesSound, 0);
		}
	}
}

void GameBoard::spawnNewTetromino( ) {
	//Ensure on startup that we have a tetromino
	if (!nextTetromino) {
		random_device dev;
		mt19937 rng(dev( ));
		uniform_int_distribution<mt19937::result_type> dist6(0, static_cast<int>(TetrominoShape::COUNT) - 1);
		TetrominoShape shape = static_cast<TetrominoShape>(dist6(rng));

		nextTetromino = make_shared<Tetromino>(shape);
	}

	currentTetromino = move(nextTetromino);
	currentTetromino->move(width / 2 - 1, 0);

	// Generate next tetromino
	random_device dev;
	mt19937 rng(dev( ));
	uniform_int_distribution<mt19937::result_type> dist6(0, static_cast<int>(TetrominoShape::COUNT) - 1);
	TetrominoShape shape = static_cast<TetrominoShape>(dist6(rng));
	nextTetromino = make_shared<Tetromino>(shape);

	if (checkCollision(*currentTetromino)) {
		collision = true;
		lockedTetrominos.clear( );
		lockedColors.clear( );
	}
}

void GameBoard::update( ) {
	if (!tryMoveCurrentTetromino(0, 1)) {
		lockTetromino( );
		clearLines( );
		spawnNewTetromino( );
	}
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
const int GameBoard::getLines( ) const { return lines; }
const shared_ptr<Tetromino> GameBoard::getNextTetromino( ) const { return nextTetromino; }


const vector<vector<int>>& GameBoard::getLockedTetrominos( ) const { return lockedTetrominos; }
const vector<vector<SDL_Color>>& GameBoard::getLockedColors( ) const { return lockedColors; }
const Tetromino& GameBoard::getCurrentTetromino( ) const { return *currentTetromino; }

void GameBoard::reset( ) {
	lockedTetrominos.clear( );
	lockedColors.clear( );
	score = 0;
	level = 1;
	collision = false;
}
