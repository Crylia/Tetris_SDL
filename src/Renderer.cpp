#include "Renderer.hpp"
#include <iostream>

Renderer::Renderer(shared_ptr<SDL_Renderer> renderer, int w, int h) : renderer(renderer), blockSize(30), windowHeight(h), windowWidth(w) {
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
	drawWall(gameBoard->getWidth( ), gameBoard->getHeight( ));
	drawLockedBlocks(gameBoard);
	drawTetromino(gameBoard->getCurrentTetromino( ));
	drawScoreboard(gameBoard->getScore( ), gameBoard->getLevel( ), gameBoard->getLines( ));
}

void Renderer::drawScoreboard(int score, int level, int lines) {
	// 6 Because the gameBoard is 10 blocks, half that is 5 + 1 for the wall = 6
	SDL_Rect blackRect = { 0, 0,  (windowWidth / 2) - (blockSize * 6) - blockSize / 8, windowHeight };
	SDL_SetRenderDrawColor(renderer.get( ), 0, 0, 0, 255);
	SDL_RenderFillRect(renderer.get( ), &blackRect);

	auto surf = unique_ptr<SDL_Surface, decltype(&SDL_FreeSurface)>(IMG_Load("assets/sprites/scoreboard.png"), SDL_FreeSurface);
	renderTexture("assets/sprites/scoreboard.png", windowWidth - surf->w * static_cast<float>(windowHeight) / surf->h, 0, surf->w * static_cast<float>(windowHeight) / surf->h, windowHeight);

	renderText(fmt::format("{0}", score), windowWidth - 30, 97, 32, SDL_Color{ 0,0,0 }, HAlign::RIGHT);
	renderText(fmt::format("{0}", level), windowWidth - 92, 230, 32, SDL_Color{ 0,0,0 });
	renderText(fmt::format("{0}", lines), windowWidth - 92, 330, 32, SDL_Color{ 0,0,0 });
}

void Renderer::drawWall(const int w, const int h) {
	int innerBorderThickness = blockSize / 4;

	for (int y = 0; y < (h + (innerBorderThickness * h)); ++y) {
		SDL_Color color{ 165, 42, 42 };
		renderTexture(textures[TetrisAssets::BORDER], offsetX - blockSize, (offsetY + y * blockSize) - (innerBorderThickness * y + 1), blockSize, blockSize, color);
		renderTexture(textures[TetrisAssets::BORDER], offsetX + w * blockSize, (offsetY + y * blockSize) - (innerBorderThickness * y + 1), blockSize, blockSize, color);
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
					offsetX + col * blockSize,
					offsetY + row * blockSize,
					blockSize,
					blockSize,
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
					offsetX + (x + i) * blockSize,
					offsetY + y * blockSize,
					blockSize,
					blockSize,
					tetromino->getColor( )
				);
			}
			renderTexture(
				textures[TetrisAssets::I_ENDR],
				offsetX + (x + 0) * blockSize,
				offsetY + y * blockSize,
				blockSize,
				blockSize,
				tetromino->getColor( )
			);
			renderTexture(
				textures[TetrisAssets::I_STARTR],
				offsetX + (x + 3) * blockSize,
				offsetY + y * blockSize,
				blockSize,
				blockSize,
				tetromino->getColor( )
			);
		} else {
			renderTexture(
				textures[TetrisAssets::I_END],
				offsetX + x * blockSize,
				offsetY + y * blockSize,
				blockSize,
				blockSize,
				tetromino->getColor( )
			);
			renderTexture(
				textures[TetrisAssets::I_MID],
				offsetX + x * blockSize,
				offsetY + (y + 1) * blockSize,
				blockSize,
				blockSize,
				tetromino->getColor( )
			);
			renderTexture(
				textures[TetrisAssets::I_MID],
				offsetX + x * blockSize,
				offsetY + (y + 2) * blockSize,
				blockSize,
				blockSize,
				tetromino->getColor( )
			);
			renderTexture(
				textures[TetrisAssets::I_START],
				offsetX + x * blockSize,
				offsetY + (y + 3) * blockSize,
				blockSize,
				blockSize,
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
						offsetX + (x + col) * blockSize,
						offsetY + (y + row) * blockSize,
						blockSize,
						blockSize,
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
	SDL_SetRenderDrawColor(renderer.get( ), 248, 248, 248, 255);
	SDL_RenderClear(renderer.get( ));

	int borderThickness = 20;
	SDL_Rect borderRect = {
			borderThickness,
			borderThickness,
			windowWidth - 2 * borderThickness,
			windowHeight - 2 * borderThickness
	};
	SDL_SetRenderDrawColor(renderer.get( ), 0, 0, 0, 255);
	SDL_RenderDrawRect(renderer.get( ), &borderRect);

	int titleWidth = static_cast<int>(windowWidth * 0.8f);
	int titleHeight = static_cast<int>(titleWidth * 0.315f);

	renderTexture(
		"assets/sprites/title.png",
		(windowWidth / 2) - (titleWidth / 2),
		static_cast<int>(windowHeight * 0.160f),
		titleWidth,
		titleHeight
	);

	auto titleBg = unique_ptr<SDL_Surface, decltype(&SDL_FreeSurface)>(IMG_Load("assets/sprites/title_bg.png"), SDL_FreeSurface);

	int titleBgWidth = static_cast<int>(windowWidth * 0.8f);
	int titleBgHeight = static_cast<int>(titleBgWidth * (static_cast<float>(titleBg->h) / titleBg->w));

	renderTexture(
		"assets/sprites/title_bg.png",
		(windowWidth / 2) - (titleBgWidth / 2),
		static_cast<int>(windowHeight * 0.5f),
		titleBgWidth,
		titleBgHeight
	);
	renderText("press G to start", (windowWidth / 2), windowHeight - 70, 16, SDL_Color{ 0, 0, 0 }, HAlign::CENTER);

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
				(windowWidth - 155) + (x + i) * blockSize,
				(windowHeight - 120) + y * blockSize,
				blockSize,
				blockSize,
				nextTetromino->getColor( )
			);
		}

		renderTexture(
			textures[TetrisAssets::I_ENDR],
			(windowWidth - 155) + x * blockSize,
			(windowHeight - 120) + y * blockSize,
			blockSize,
			blockSize,
			nextTetromino->getColor( )
		);

		renderTexture(
			textures[TetrisAssets::I_STARTR],
			(windowWidth - 155) + (x + 3) * blockSize,
			(windowHeight - 120) + y * blockSize,
			blockSize,
			blockSize,
			nextTetromino->getColor( )
		);

	} else {
		for (int row = 0; row < nextTetromino->getShape( ).size( ); ++row) {
			for (int col = 0; col < nextTetromino->getShape( )[row].size( ); ++col) {
				if (nextTetromino->getShape( )[row][col] != 0) {
					renderTexture(
						textures[shapeToAsset(nextTetromino->getShapeEnumn( ))],
						(windowWidth - 140) + col * blockSize,
						(windowHeight - 130) + row * blockSize,
						blockSize,
						blockSize,
						nextTetromino->getColor( )
					);
				}
			}
		}
	}
}

const Renderer::TextDimensions Renderer::renderText(const string& text, int x, int y, int fontSize, SDL_Color color, HAlign hAlign, VAlign vAlign) {
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

const Renderer::TextureDimensions Renderer::renderTexture(
	const string& texturePath, int x, int y, int width, int height,
	SDL_Color color, float scale, HAlign textHAlign, VAlign textVAlign) {

	auto surface = unique_ptr <SDL_Surface, decltype(&SDL_FreeSurface)>(IMG_Load(texturePath.c_str( )), SDL_FreeSurface);
	if (!surface) { SDL_Log("Failed to load surface from %s: %s", texturePath, SDL_GetError( ));return{ 0,0,0,0 }; }

	auto texture = unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)>(SDL_CreateTextureFromSurface(renderer.get( ), surface.get( )), SDL_DestroyTexture);
	if (!texture) { SDL_Log("Failed to create texture from surface: %s", SDL_GetError( )); return{ 0,0,0,0 }; }

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

	return { x,y,textureWidth, textureHeight };
}

const int Renderer::getBlockSize( ) const { return blockSize; }

void Renderer::setBlockSize(int newBlockSize) { blockSize = newBlockSize; }

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

