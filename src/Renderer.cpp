#include "Renderer.hpp"
#include <iostream>
#include <cmath>

Renderer::Renderer(shared_ptr<SDL_Renderer> renderer, int w, int h) : renderer(renderer), windowHeight(h), windowWidth(w) {
	textures[TetrisAssets::SINGLE] = "assets/sprites/single.png";
	textures[TetrisAssets::BORDER] = "assets/sprites/border.png";
	textures[TetrisAssets::J] = "assets/sprites/J.png";
	textures[TetrisAssets::L] = "assets/sprites/L.png";
	textures[TetrisAssets::T] = "assets/sprites/T.png";
	textures[TetrisAssets::O] = "assets/sprites/O.png";
	textures[TetrisAssets::S] = "assets/sprites/S.png";
	textures[TetrisAssets::Z] = "assets/sprites/Z.png";
	textures[TetrisAssets::I] = "assets/sprites/I.png";
	textures[TetrisAssets::I_END] = "assets/sprites/I_END.png";
	textures[TetrisAssets::I_MID] = "assets/sprites/I_MID.png";
	textures[TetrisAssets::I_START] = "assets/sprites/I_START.png";
	textures[TetrisAssets::I_ENDR] = "assets/sprites/I_ENDR.png";
	textures[TetrisAssets::I_MIDR] = "assets/sprites/I_MIDR.png";
	textures[TetrisAssets::I_STARTR] = "assets/sprites/I_STARTR.png";
}

void Renderer::renderBoard(const shared_ptr<GameBoard> gameBoard) {
	drawScoreboard(gameBoard->getScore( ), gameBoard->getLevel( ), gameBoard->getLines( ));
	drawWall(gameBoard->getWidth( ), gameBoard->getHeight( ));
	drawLockedBlocks(gameBoard);
	drawTetromino(gameBoard->getCurrentTetromino( ));
}

void Renderer::drawScoreboard(int score, int level, int lines) {
	SDL_Rect blackRect = { 0, 0,  7 * scale, windowHeight };
	SDL_SetRenderDrawColor(renderer.get( ), 0, 0, 0, 255);
	SDL_RenderFillRect(renderer.get( ), &blackRect);
	SDL_Color col{ 255,255,255 };
	auto surf = unique_ptr<SDL_Surface, decltype(&SDL_FreeSurface)>(IMG_Load("assets/sprites/scoreboard.png"), SDL_FreeSurface);
	scoreBoardDimensions = renderTexture(
		"assets/sprites/scoreboard.png",
		windowWidth,
		0,
		surf->w * scale,
		surf->h * scale,
		col,
		1.0f,
		HAlign::RIGHT
	);

	renderText(
		fmt::format("{0}", score),
		windowWidth - (7 * scale),
		23 * scale,
		8 * scale,
		SDL_Color{ 0,0,0 },
		HAlign::RIGHT,
		VAlign::TOP
	);
	renderText(
		fmt::format("{0}", level),
		windowWidth - (15 * scale),
		55 * scale,
		8 * scale,
		SDL_Color{ 0,0,0 },
		HAlign::RIGHT,
		VAlign::TOP
	);
	renderText(
		fmt::format("{0}", lines),
		windowWidth - (15 * scale),
		79 * scale,
		8 * scale,
		SDL_Color{ 0,0,0 },
		HAlign::RIGHT,
		VAlign::TOP
	);
}

void Renderer::drawWall(const int w, const int h) {
	SDL_Color color{ 165, 42, 42 }, gapColor{ 0,0,0 };
	SDL_SetRenderDrawColor(renderer.get( ), color.r, color.g, color.b, 255);

	for (int y = 0; y <= windowHeight; y += ((gridSize - 2) * scale)) {
		leftBorder = renderTexture(
			textures[TetrisAssets::BORDER],
			gridSize * scale,
			y,
			gridSize * scale,
			(gridSize * scale),
			color
		);
		int t = ceil(static_cast<float>(scoreBoardDimensions->w) / scale / gridSize);
		rightBorder = renderTexture(
			textures[TetrisAssets::BORDER],
			windowWidth - (ceil(static_cast<float>(scoreBoardDimensions->w) / scale / gridSize) + 1) * 8 * scale,
			y,
			gridSize * scale,
			gridSize * scale,
			color
		);
	}
}

void Renderer::drawLockedBlocks(const shared_ptr<GameBoard> gameBoard) {
	const auto& lockedTetrominos = gameBoard->getLockedTetrominos( );
	const auto& lockedColors = gameBoard->getLockedColors( );

	for (int row = 0; row < lockedTetrominos.size( ); ++row) {
		for (int col = 0; col < lockedTetrominos[row].size( ); ++col) {
			int blockType = lockedTetrominos[row][col];
			if (blockType != 0) {
				SDL_Color color = lockedColors[row][col];
				renderTexture(
					textures[shapeToAsset(static_cast<TetrominoShape>(blockType - 1))],
					(col + 2) * gridSize * scale,
					row * gridSize * scale,
					gridSize * scale,
					gridSize * scale,
					color);
			}
		}
	}
}

void Renderer::drawTetromino(const shared_ptr<Tetromino> tetromino) {
	if (!tetromino) return;

	int x = tetromino->getX( ), y = tetromino->getY( );

	if (tetromino->getShapeEnumn( ) == TetrominoShape::I) {
		double angle = tetromino->getRotationAngle( );

		if (angle == 90 || angle == 270) {
			for (int i = 0; i < 4; ++i) {
				renderTexture(
					textures[TetrisAssets::I_MIDR],
					(2 + x + i) * gridSize * scale,
					y * gridSize * scale,
					gridSize * scale,
					gridSize * scale,
					tetromino->getColor( )
				);
			}
			renderTexture(
				textures[TetrisAssets::I_ENDR],
				(2 + x) * gridSize * scale,
				y * gridSize * scale,
				gridSize * scale,
				gridSize * scale,
				tetromino->getColor( )
			);
			renderTexture(
				textures[TetrisAssets::I_STARTR],
				(2 + x + 3) * gridSize * scale,
				y * gridSize * scale,
				gridSize * scale,
				gridSize * scale,
				tetromino->getColor( )
			);
		} else {
			renderTexture(
				textures[TetrisAssets::I_END],
				(2 + x) * gridSize * scale,
				y * gridSize * scale,
				gridSize * scale,
				gridSize * scale,
				tetromino->getColor( )
			);
			renderTexture(
				textures[TetrisAssets::I_MID],
				(2 + x) * gridSize * scale,
				(y + 1) * gridSize * scale,
				gridSize * scale,
				gridSize * scale,
				tetromino->getColor( )
			);
			renderTexture(
				textures[TetrisAssets::I_MID],
				(2 + x) * gridSize * scale,
				(y + 2) * gridSize * scale,
				gridSize * scale,
				gridSize * scale,
				tetromino->getColor( )
			);
			renderTexture(
				textures[TetrisAssets::I_START],
				(2 + x) * gridSize * scale,
				(y + 3) * gridSize * scale,
				gridSize * scale,
				gridSize * scale,
				tetromino->getColor( )
			);
		}
	} else {
		const auto& shape = tetromino->getShape( );
		for (int row = 0; row < shape.size( ); ++row) {
			for (int col = 0; col < shape[row].size( ); ++col) {
				if (shape[row][col] != 0) {
					renderTexture(
						textures[shapeToAsset(tetromino->getShapeEnumn( ))],
						((x + col) * gridSize * scale) + leftBorder->x + leftBorder->w,
						(y + row) * gridSize * scale,
						gridSize * scale,
						gridSize * scale,
						tetromino->getColor( )
					);
				}
			}
		}
	}
}

const TetrisAssets Renderer::shapeToAsset(const TetrominoShape shape) const {
	switch (shape) {
	case TetrominoShape::I:
		return TetrisAssets::I;
	case TetrominoShape::O:
		return TetrisAssets::O;
	case TetrominoShape::T:
		return TetrisAssets::T;
	case TetrominoShape::J:
		return TetrisAssets::J;
	case TetrominoShape::L:
		return TetrisAssets::L;
	case TetrominoShape::S:
		return TetrisAssets::S;
	case TetrominoShape::Z:
		return TetrisAssets::Z;
	case TetrominoShape::I_END:
		return TetrisAssets::I_END;
	case TetrominoShape::I_START:
		return TetrisAssets::I_START;
	case TetrominoShape::I_MID:
		return TetrisAssets::I_MID;
	case TetrominoShape::I_ENDR:
		return TetrisAssets::I_ENDR;
	case TetrominoShape::I_STARTR:
		return TetrisAssets::I_STARTR;
	case TetrominoShape::I_MIDR:
		return TetrisAssets::I_MIDR;
	default:
		return TetrisAssets::I;
		break;
	}
}

void Renderer::renderStartScreen( ) {
	SDL_SetRenderDrawColor(renderer.get( ), 0, 0, 0, 255);
	SDL_RenderClear(renderer.get( ));

	auto title = unique_ptr<SDL_Surface, decltype(&SDL_FreeSurface)>(IMG_Load("assets/sprites/title.png"), SDL_FreeSurface);

	int titlePaddingX = 3, titlePaddingY = 8;

	TextureDimensions* titleDimensions = renderTexture(
		"assets/sprites/title.png",
		titlePaddingX * scale,
		titlePaddingY * scale,
		title->w * scale,
		title->h * scale
	);

	auto titleBg = unique_ptr<SDL_Surface, decltype(&SDL_FreeSurface)>(IMG_Load("assets/sprites/title_bg.png"), SDL_FreeSurface);

	SDL_Color col = { 255,255,255 };

	TextureDimensions* titleBgDimensions = renderTexture(
		"assets/sprites/title_bg.png",
		windowWidth / 2,
		titleDimensions->y + titleDimensions->h,
		titleBg->w * scale,
		titleBg->h * scale,
		col,
		1.0f,
		HAlign::CENTER
	);

	SDL_SetRenderDrawColor(renderer.get( ), 248, 248, 248, 255);

	int titleBgBottomPadding = 6;
	int y = titleBgDimensions->y + titleBgDimensions->h + titleBgBottomPadding * scale;

	SDL_Rect rect{
		0,
		y,
		windowWidth,
		windowHeight - y,
	};

	SDL_RenderFillRect(renderer.get( ), &rect);

	TextDimensions player1TextDimendions = renderText(
		"1player",
		windowWidth / 4,
		y + (1 * scale),
		8 * scale,
		SDL_Color{ 0, 0, 0 },
		HAlign::CENTER
	);

	renderText(
		"2player",
		windowWidth * 3 / 4,
		y + (1 * scale),
		8 * scale,
		SDL_Color{ 0, 0, 0 },
		HAlign::CENTER
	);

	renderText(
		"©1989 ®",
		windowWidth / 2,
		windowHeight - (14 * scale),
		8 * scale,
		SDL_Color{ 0, 0, 0 },
		HAlign::CENTER
	);

	SDL_RenderPresent(renderer.get( ));
}

void Renderer::renderGameOver(const shared_ptr<GameBoard> gameBoard) {
	//Needed to draw the Walls again
	drawWall(gameBoard->getWidth( ), gameBoard->getHeight( ));
	drawScoreboard(gameBoard->getScore( ), gameBoard->getLevel( ), gameBoard->getLines( ));

	auto gameOver = unique_ptr<SDL_Surface, decltype(&SDL_FreeSurface)>(IMG_Load("assets/sprites/game_over.png"), SDL_FreeSurface);

	int gameOverWidth = static_cast<int>(windowWidth * 0.3f);
	int gameOverHeight = static_cast<int>(gameOverWidth * (static_cast<float>(gameOver->h) / gameOver->w));

	renderTexture(
		"assets/sprites/game_over.png",
		(windowWidth / 2) - (gameOverWidth / 2),
		static_cast<int>(windowHeight * 0.15f),
		gameOverWidth,
		gameOverHeight
	);

	SDL_Color col{ 255,255,255 };
	renderTexture("assets/sprites/please_try_again_text.png", windowWidth / 2, windowHeight * 0.7f, 0, 0, col, 3.5f, HAlign::CENTER);

	SDL_RenderPresent(renderer.get( ));
}

void Renderer::renderTetrominoPreview(const shared_ptr<Tetromino> nextTetromino) {
	if (!nextTetromino) return;

	int x = nextTetromino->getX( ), y = nextTetromino->getY( );

	if (nextTetromino->getShapeEnumn( ) == TetrominoShape::I) {
		for (int i = 0; i < 4; ++i) {
			renderTexture(
				textures[TetrisAssets::I_MIDR],
				(windowWidth - 155) + (x + i) * gridSize * scale,
				(windowHeight - 120) + y * gridSize * scale,
				gridSize * scale,
				gridSize * scale,
				nextTetromino->getColor( )
			);
		}

		renderTexture(
			textures[TetrisAssets::I_ENDR],
			(windowWidth - 155) + x * gridSize * scale,
			(windowHeight - 120) + y * gridSize * scale,
			gridSize * scale,
			gridSize * scale,
			nextTetromino->getColor( )
		);

		renderTexture(
			textures[TetrisAssets::I_STARTR],
			(windowWidth - 155) + (x + 3) * gridSize * scale,
			(windowHeight - 120) + y * gridSize * scale,
			gridSize * scale,
			gridSize * scale,
			nextTetromino->getColor( )
		);

	} else {
		for (int row = 0; row < nextTetromino->getShape( ).size( ); ++row) {
			for (int col = 0; col < nextTetromino->getShape( )[row].size( ); ++col) {
				if (nextTetromino->getShape( )[row][col] != 0) {
					renderTexture(
						textures[shapeToAsset(nextTetromino->getShapeEnumn( ))],
						(windowWidth - 140) + col * gridSize * scale,
						(windowHeight - 130) + row * gridSize * scale,
						gridSize * scale,
						gridSize * scale,
						nextTetromino->getColor( )
					);
				}
			}
		}
	}
}

Renderer::TextDimensions Renderer::renderText(const string& text, int x, int y, int fontSize, SDL_Color color, HAlign hAlign, VAlign vAlign) {
	auto font = unique_ptr<TTF_Font, decltype(&TTF_CloseFont)>(TTF_OpenFont("assets/font/tetris-gb.ttf", fontSize), TTF_CloseFont);
	if (!font) { SDL_Log("Failed to create font: %s", TTF_GetError( )); return{ 0,0,0,0 }; }

	auto surface = unique_ptr<SDL_Surface, decltype(&SDL_FreeSurface)>(TTF_RenderText_Solid(font.get( ), text.c_str( ), color), SDL_FreeSurface);
	if (!surface) { SDL_Log("Failed to create surface: %s", TTF_GetError( ));return{ 0,0,0,0 }; }

	auto texture = unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)>(SDL_CreateTextureFromSurface(renderer.get( ), surface.get( )), SDL_DestroyTexture);
	if (!texture) { SDL_Log("Failed to create texture from surface: %s", SDL_GetError( ));return{ 0,0,0,0 }; }

	int width = surface->w;
	int height = surface->h;

	if (hAlign == HAlign::CENTER)
		x -= width / 2;
	else if (hAlign == HAlign::RIGHT)
		x -= width;

	if (vAlign == VAlign::CENTER)
		y -= height / 2;
	else if (vAlign == VAlign::BOTTOM)
		y -= height;


	SDL_Rect destRect = { x, y, width, height };
	SDL_RenderCopy(renderer.get( ), texture.get( ), nullptr, &destRect);

	return TextDimensions{ x, y, width, height };
}

Renderer::TextureDimensions* Renderer::renderTexture(
	const string& texturePath, int x, int y, int width, int height,
	SDL_Color color, float scale, HAlign textHAlign, VAlign textVAlign) {

	auto surface = unique_ptr <SDL_Surface, decltype(&SDL_FreeSurface)>(IMG_Load(texturePath.c_str( )), SDL_FreeSurface);
	if (!surface) { SDL_Log("Failed to load surface from %s: %s", texturePath, SDL_GetError( ));return nullptr; }

	auto texture = unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)>(SDL_CreateTextureFromSurface(renderer.get( ), surface.get( )), SDL_DestroyTexture);
	if (!texture) { SDL_Log("Failed to create texture from surface: %s", SDL_GetError( )); return nullptr; }

	SDL_SetTextureBlendMode(texture.get( ), SDL_BLENDMODE_BLEND);
	SDL_SetTextureColorMod(texture.get( ), color.r, color.g, color.b);

	int textureWidth = static_cast<int>((width == 0 ? surface->w : width) * scale);
	int textureHeight = static_cast<int>((height == 0 ? surface->h : height) * scale);

	if (textHAlign == HAlign::CENTER)
		x -= textureWidth / 2;
	else if (textHAlign == HAlign::RIGHT)
		x -= textureWidth;


	if (textVAlign == VAlign::CENTER)
		y -= textureHeight / 2;
	else if (textVAlign == VAlign::BOTTOM)
		y -= textureHeight;

	SDL_Rect rect{ x,y,textureWidth,textureHeight };
	SDL_RenderCopy(renderer.get( ), texture.get( ), nullptr, &rect);

	return new TextureDimensions{ x,y,textureWidth, textureHeight };
}

const int Renderer::getScale( ) const { return scale; }

void Renderer::setScale(int newScale) { scale = newScale; }

void Renderer::setOffset(int newX, int newY) {
	offsetX = newX;
	offsetY = newY;
}

const int Renderer::getOffsetX( ) const { return offsetX; }

const int Renderer::getOffsetY( ) const { return offsetY; }

void Renderer::setWindowSize(int w, int h) {
	windowWidth = w;
	windowHeight = h;
}

