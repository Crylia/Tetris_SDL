#pragma once

extern "C" {
#include <SDL2/SDL.h>
}

#include <vector>
#include <string>
#include <random>

using namespace std;

enum class TetrominoShape {
	L,
	I,
	O,
	S,
	Z,
	J,
	COUNT //Used to know the ammount of shapes
};

class GameBoard;

class Tetromino {
private:
	void initializeShape(TetrominoShape shape);
	vector<vector<int>> shape;

	int x, y;

	int currentRotationState;

	TetrominoShape textureShape;

	SDL_Color color;
public:
	Tetromino(TetrominoShape shape);

	void rotate(GameBoard& gameBoard);
	void move(int dx, int dy);
	double getRotationAngle( ) const;

	const vector<vector<int>>& getShape( ) const;
	int getX( ) const;
	int getY( ) const;

	void setTexture(const TetrominoShape textureName);
	const TetrominoShape getTexture( ) const;
	SDL_Color getColor( ) const;
};
