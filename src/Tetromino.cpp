#include "Tetromino.hpp"
#include "GameBoard.hpp"

Tetromino::Tetromino(TetrominoShape shape) : x(0), y(0), textureShape(shape) {
	initializeShape(shape);
	currentRotationState = 1;

	color.a = 255;
	color.r = 0;
	color.g = 0;
	color.b = 0;

	random_device dev;
	mt19937 rng(dev( ));
	uniform_int_distribution<mt19937::result_type> dist6(0, 5);

	switch (dist6(rng)) {
	case 0:
		color.r = 0;
		color.g = 191;
		color.b = 255;
		break;
	case 1:
		color.r = 255;
		color.g = 215;
		color.b = 0;
		break;
	case 2:
		color.r = 138;
		color.g = 43;
		color.b = 226;
		break;
	case 3:
		color.r = 0;
		color.g = 204;
		color.b = 102;
		break;
	case 4:
		color.r = 255;
		color.g = 69;
		color.b = 0;
		break;
	case 5:
		color.r = 30;
		color.g = 144;
		color.b = 255;
		break;
	case 6:
		color.r = 255;
		color.g = 140;
		color.b = 0;
		break;
	default:
		break;
	}

}

void Tetromino::initializeShape(TetrominoShape s) {
	switch (s) {
	case TetrominoShape::I:
		shape = { {1, 1, 1, 1} };
		break;
	case TetrominoShape::O:
		shape = { {1, 1}, {1, 1} };
		break;
	case TetrominoShape::S:
		shape = { {0, 1, 1}, {1, 1, 0} };
		break;
	case TetrominoShape::Z:
		shape = { {1, 1, 0}, {0, 1, 1} };
		break;
	case TetrominoShape::J:
		shape = { {1, 0, 0}, {1, 1, 1} };
		break;
	case TetrominoShape::L:
		shape = { {0, 0, 1}, {1, 1, 1} };
		break;
	case TetrominoShape::T:
		shape = { {1, 1, 1}, {0, 1, 0} };
		break;
	default:
		shape = { {0} };
		break;
	}
}

void Tetromino::rotate(GameBoard& gameBoard) {
	vector<vector<int>> rotated(shape[0].size( ), vector<int>(shape.size( )));
	for (int row = 0; row < shape.size( ); ++row)
		for (int col = 0; col < shape[0].size( ); ++col)
			rotated[col][shape.size( ) - 1 - row] = shape[row][col];

	if (gameBoard.isValidPosition(rotated, x, y)) {
		shape = rotated;
		currentRotationState = (currentRotationState + 1) % 4;
	} else {
		int dx = x - 1;
		while (!gameBoard.isValidPosition(rotated, dx, y)) {
			//If the shape before rotation wont have a valid position then rotation is not possible and we abort
			if (!gameBoard.isValidPosition(shape, dx, y)) {
				shape = rotated;
				return;
			}
			dx--;
		}
		x = dx;
		shape = rotated;
		currentRotationState = (currentRotationState + 1) % 4;
	}
}

void Tetromino::move(int dx, int dy) {
	x += dx;
	y += dy;
}

double Tetromino::getRotationAngle( ) const {
	return currentRotationState * 90.0;
}

const vector<vector<int>>& Tetromino::getShape( ) const { return shape; }
const TetrominoShape Tetromino::getShapeEnumn( ) const { return textureShape; }

int Tetromino::getX( ) const { return x; }
int Tetromino::getY( ) const { return y; }

SDL_Color Tetromino::getColor( ) const { return color; }
