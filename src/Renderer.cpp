#include "Renderer.hpp"
#include <iostream>

Renderer::Renderer(shared_ptr<SDL_Renderer> renderer, int w, int h) : renderer(renderer), blockSize(30), windowHeight(h), windowWidth(w) {
	setRenderer(renderer);
}

void Renderer::setRenderer(shared_ptr<SDL_Renderer> renderer) {
	this->renderer = renderer;
	loadTexture("assets/single.png", TetrisAssets::SINGLE);
	loadTexture("assets/border.png", TetrisAssets::BORDER);
	loadTexture("assets/J.png", TetrisAssets::J);
	loadTexture("assets/L.png", TetrisAssets::L);
	loadTexture("assets/T.png", TetrisAssets::T);
	loadTexture("assets/O.png", TetrisAssets::O);
	loadTexture("assets/S.png", TetrisAssets::S);
	loadTexture("assets/Z.png", TetrisAssets::Z);
	loadTexture("assets/I_MID.png", TetrisAssets::I);
	loadTexture("assets/I_END.png", TetrisAssets::I_END);
	loadTexture("assets/I_MID.png", TetrisAssets::I_MID);
	loadTexture("assets/I_START.png", TetrisAssets::I_START);
	loadTexture("assets/I_ENDR.png", TetrisAssets::I_ENDR);
	loadTexture("assets/I_MIDR.png", TetrisAssets::I_MIDR);
	loadTexture("assets/I_STARTR.png", TetrisAssets::I_STARTR);
}

void Renderer::renderBoard(const GameBoard& gameBoard) {
	drawGrid(gameBoard);
	drawLockedBlocks(gameBoard);
	drawTetromino(gameBoard.getCurrentTetromino( ));
	drawScoreboard(gameBoard.getScore( ), gameBoard.getLevel( ), gameBoard.getLines( ));
}

void Renderer::drawScoreboard(int score, int level, int lines) {
	int gapSize = blockSize / 8;

	// 6 Because the gameBoard is 10 blocks, half that is 5 + 1 for the wall = 6
	int blackAreaWidth = (windowWidth / 2) - (blockSize * 6) - gapSize;

	SDL_Rect blackRect = { 0, 0, blackAreaWidth, windowHeight };
	SDL_SetRenderDrawColor(renderer.get( ), 0, 0, 0, 255);
	SDL_RenderFillRect(renderer.get( ), &blackRect);

	SDL_Surface* scoreboardSurface = IMG_Load("assets/scoreboard.png");
	if (!scoreboardSurface) {
		SDL_Log("Failed to load title surface: %s", SDL_GetError( ));
		return;
	}

	auto scoreboardTexture = SDL_CreateTextureFromSurface(renderer.get( ), scoreboardSurface);
	SDL_FreeSurface(scoreboardSurface);
	if (!scoreboardTexture) {
		SDL_Log("Failed to create title texture: %s", SDL_GetError( ));
		return;
	}

	int width, height;
	SDL_QueryTexture(scoreboardTexture, nullptr, nullptr, &width, &height);

	int scoreboardWidth = static_cast<int>(width * static_cast<float>(windowHeight) / height);

	SDL_Rect scoreboardRect = {
			windowWidth - scoreboardWidth,
			0,
			scoreboardWidth,
			windowHeight
	};


	SDL_RenderCopy(renderer.get( ), scoreboardTexture, nullptr, &scoreboardRect);
	SDL_DestroyTexture(scoreboardTexture);

	renderRightAlignedText(fmt::format("{0}", score), windowWidth - 30, 97, 32, SDL_Color{ 0,0,0 });
	renderText(fmt::format("{0}", level), windowWidth - 92, 230, 32, SDL_Color{ 0,0,0 });
	renderText(fmt::format("{0}", lines), windowWidth - 92, 330, 32, SDL_Color{ 0,0,0 });
}

void Renderer::renderRightAlignedText(const std::string& text, int x, int y, int fontSize, const SDL_Color& color) {
	TTF_Font* font = TTF_OpenFont("assets/tetris-gb.ttf", fontSize);
	if (!font) {
		SDL_Log("Failed to open font: %s", TTF_GetError( ));
		return;
	}

	int textWidth;
	TTF_SizeText(font, text.c_str( ), &textWidth, nullptr);

	int rightAlignedX = x - textWidth;

	renderText(text, rightAlignedX, y, fontSize, color);

	TTF_CloseFont(font);
}

void Renderer::drawGrid(const GameBoard& board) {
	int borderThickness = 10, gridThickness = 1;
	int innerBorderThickness = blockSize / 4;

	int w = board.getLockedTetrominos( )[0].size( );
	int h = board.getLockedTetrominos( ).size( );

	int totalWidth = w * blockSize;
	int totalHeight = h * blockSize;

	SDL_SetTextureBlendMode(textures[TetrisAssets::BORDER], SDL_BLENDMODE_BLEND);
	SDL_SetTextureColorMod(textures[TetrisAssets::BORDER], 165, 42, 42);

	for (int y = 0; y < (h + (innerBorderThickness * h)); ++y) {
		SDL_Rect leftBlock = { offsetX - blockSize, offsetY + y * blockSize, blockSize, blockSize };
		renderTexture(TetrisAssets::BORDER, leftBlock.x, leftBlock.y - (innerBorderThickness * y + 1), leftBlock.w, leftBlock.h);

		SDL_Rect rightBlock = { offsetX + totalWidth, offsetY + y * blockSize, blockSize, blockSize };
		renderTexture(TetrisAssets::BORDER, rightBlock.x, rightBlock.y - (innerBorderThickness * y + 1), rightBlock.w, rightBlock.h);
	}
}

void Renderer::drawLockedBlocks(const GameBoard& gameBoard) {
	const auto& lockedTetrominos = gameBoard.getLockedTetrominos( );
	const auto& lockedColors = gameBoard.getLockedColors( );

	for (int row = 0; row < lockedTetrominos.size( ); ++row) {
		for (int col = 0; col < lockedTetrominos[row].size( ); ++col) {
			int blockType = lockedTetrominos[row][col];
			if (blockType != 0) {
				SDL_Rect block = { offsetX + col * blockSize, offsetY + row * blockSize, blockSize, blockSize };

				TetrominoShape shape = static_cast<TetrominoShape>(blockType - 1);
				TetrisAssets asset = shapeToAsset(shape);

				if (textures[asset]) {
					SDL_Color color = lockedColors[row][col];
					SDL_SetTextureBlendMode(textures[asset], SDL_BLENDMODE_BLEND);
					SDL_SetTextureColorMod(textures[asset], color.r, color.g, color.b);
					renderTexture(asset, block.x, block.y, block.w, block.h);
				} else {
					SDL_Log("Texture not found for asset: %d", asset);
					SDL_SetRenderDrawColor(renderer.get( ), 255, 0, 0, 255);
					SDL_RenderFillRect(renderer.get( ), &block);
				}
			}
		}
	}
}

TetrisAssets Renderer::shapeToAsset(const TetrominoShape shape) {
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
		break;
	}
}

void Renderer::drawTetromino(const Tetromino& tetromino) {
	const auto& shape = tetromino.getShape( );
	int x = tetromino.getX( ), y = tetromino.getY( );

	double angle = tetromino.getRotationAngle( );

	TetrominoShape tetrominoShape = tetromino.getShapeEnumn( );

	SDL_SetTextureBlendMode(textures[shapeToAsset(tetrominoShape)], SDL_BLENDMODE_BLEND);
	SDL_Color color = tetromino.getColor( );
	SDL_SetTextureColorMod(textures[shapeToAsset(tetrominoShape)], color.r, color.g, color.b);

	if (tetrominoShape == TetrominoShape::I) {
		if (angle == 90 || angle == 270) {
			SDL_SetTextureColorMod(textures[TetrisAssets::I_STARTR], color.r, color.g, color.b);
			SDL_SetTextureColorMod(textures[TetrisAssets::I_MIDR], color.r, color.g, color.b);
			SDL_SetTextureColorMod(textures[TetrisAssets::I_ENDR], color.r, color.g, color.b);
			for (int i = 0; i < 4; ++i) {
				SDL_Rect destRect = { offsetX + (x + i) * blockSize, offsetY + y * blockSize, blockSize, blockSize };
				SDL_RenderCopy(renderer.get( ), textures[TetrisAssets::I_MIDR], nullptr, &destRect);
			}

			SDL_Rect startRect = { offsetX + (x + 0) * blockSize, offsetY + y * blockSize, blockSize, blockSize };
			SDL_RenderCopy(renderer.get( ), textures[TetrisAssets::I_ENDR], nullptr, &startRect);

			SDL_Rect endRect = { offsetX + (x + 3) * blockSize, offsetY + y * blockSize, blockSize, blockSize };
			SDL_RenderCopy(renderer.get( ), textures[TetrisAssets::I_STARTR], nullptr, &endRect);
		} else {
			SDL_SetTextureColorMod(textures[TetrisAssets::I_START], color.r, color.g, color.b);
			SDL_SetTextureColorMod(textures[TetrisAssets::I_MID], color.r, color.g, color.b);
			SDL_SetTextureColorMod(textures[TetrisAssets::I_END], color.r, color.g, color.b);

			SDL_Rect destRect1 = { offsetX + (x + 0) * blockSize, offsetY + (y + 0) * blockSize, blockSize, blockSize };
			SDL_Rect destRect2 = { offsetX + (x + 0) * blockSize, offsetY + (y + 1) * blockSize, blockSize, blockSize };
			SDL_Rect destRect3 = { offsetX + (x + 0) * blockSize, offsetY + (y + 2) * blockSize, blockSize, blockSize };
			SDL_Rect destRect4 = { offsetX + (x + 0) * blockSize, offsetY + (y + 3) * blockSize, blockSize, blockSize };

			renderTexture(TetrisAssets::I_END, destRect1.x, destRect1.y, destRect1.w, destRect1.h);
			renderTexture(TetrisAssets::I_MID, destRect2.x, destRect2.y, destRect2.w, destRect2.h);
			renderTexture(TetrisAssets::I_MID, destRect3.x, destRect3.y, destRect3.w, destRect3.h);
			renderTexture(TetrisAssets::I_START, destRect4.x, destRect4.y, destRect4.w, destRect4.h);
		}
	} else {
		for (int row = 0; row < shape.size( ); ++row) {
			for (int col = 0; col < shape[row].size( ); ++col) {
				if (shape[row][col] != 0) {
					SDL_Rect destRect = { offsetX + (x + col) * blockSize, offsetY + (y + row) * blockSize, blockSize, blockSize };
					SDL_Point center = { blockSize / 2, blockSize / 2 };
					SDL_RenderCopy(renderer.get( ), textures[shapeToAsset(tetrominoShape)], nullptr, &destRect);
				}
			}
		}
	}
}

bool Renderer::loadTexture(const string& filePath, const TetrisAssets asset) {
	SDL_Texture* texture = IMG_LoadTexture(renderer.get( ), filePath.c_str( ));
	if (!texture) {
		SDL_Log("Failed to load Texture: %s\n%s", filePath, SDL_GetError( ));
		return false;
	}
	textures[asset] = texture;
	return true;
}

void Renderer::renderTexture(const TetrisAssets asset, int x, int y, int width, int height) {
	SDL_Rect destRect{ x,y,width,height };
	SDL_RenderCopy(renderer.get( ), textures[asset], nullptr, &destRect);
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

	SDL_Surface* titleSurface = IMG_Load("assets/title.png");
	if (!titleSurface) {
		SDL_Log("Failed to load title surface: %s", SDL_GetError( ));
		return;
	}

	auto titleTexture = SDL_CreateTextureFromSurface(renderer.get( ), titleSurface);
	SDL_FreeSurface(titleSurface);
	if (!titleTexture) {
		SDL_Log("Failed to create title texture: %s", SDL_GetError( ));
		return;
	}

	float titleScaleFactor = 0.5f;
	int titleWidth = static_cast<int>(windowWidth * titleScaleFactor);
	int titleHeight = static_cast<int>(titleWidth * 0.315f);

	SDL_Rect titleRect = {
			(windowWidth / 2) - (titleWidth / 2),
			static_cast<int>(windowHeight * 0.2f),
			titleWidth,
			titleHeight
	};

	SDL_RenderCopy(renderer.get( ), titleTexture, nullptr, &titleRect);
	SDL_DestroyTexture(titleTexture);

	SDL_Surface* titleBg = IMG_Load("assets/title_bg.png");
	if (!titleBg) {
		SDL_Log("Failed to load background surface: %s", SDL_GetError( ));
		return;
	}

	auto titleBgTexture = SDL_CreateTextureFromSurface(renderer.get( ), titleBg);
	int originalBgWidth = titleBg->w;
	int originalBgHeight = titleBg->h;
	SDL_FreeSurface(titleBg);
	if (!titleBgTexture) {
		SDL_Log("Failed to create background texture: %s", SDL_GetError( ));
		return;
	}

	float bgScaleFactor = 0.8f;
	int titleBgWidth = static_cast<int>(windowWidth * bgScaleFactor);
	int titleBgHeight = static_cast<int>(titleBgWidth * (static_cast<float>(originalBgHeight) / originalBgWidth));
	if (titleBgHeight > windowHeight * 0.4f) {
		titleBgHeight = static_cast<int>(windowHeight * 0.4f);
		titleBgWidth = static_cast<int>(titleBgHeight * (static_cast<float>(originalBgWidth) / originalBgHeight));
	}

	SDL_Rect titleBgRect = {
			(windowWidth / 2) - (titleBgWidth / 2),
			static_cast<int>(windowHeight * 0.5f),
			titleBgWidth,
			titleBgHeight
	};

	SDL_RenderCopy(renderer.get( ), titleBgTexture, nullptr, &titleBgRect);
	SDL_DestroyTexture(titleBgTexture);

	int w, h;
	TTF_Font* font = TTF_OpenFont("assets/tetris-gb.ttf", 16);
	TTF_SizeText(font, "PRESS START", &w, &h);
	TTF_CloseFont(font);

	renderText("press G to start", (windowWidth / 2) - (w / 2), windowHeight - 70, 16, SDL_Color{ 0, 0, 0 });

	SDL_RenderPresent(renderer.get( ));
}

void Renderer::renderGameOver(int score, int level, int lines) {
	int fontSize = 24;
	int lineSpacing = fontSize + 10;

	int totalHeight = 5 * lineSpacing;

	int centerX = (windowWidth / 2);
	int centerY = (windowHeight / 2) - (totalHeight / 2);

	renderText("game", centerX, centerY, fontSize, SDL_Color{ 0, 0, 0 });
	renderText("over", centerX, centerY + lineSpacing, fontSize, SDL_Color{ 0, 0, 0 });
	renderText("please", centerX, centerY + 2 * lineSpacing, fontSize, SDL_Color{ 0, 0, 0 });
	renderText("try", centerX, centerY + 3 * lineSpacing, fontSize, SDL_Color{ 0, 0, 0 });
	renderText("again @", centerX, centerY + 4 * lineSpacing, fontSize, SDL_Color{ 0, 0, 0 });

	SDL_RenderPresent(renderer.get( ));
}

void Renderer::renderTetrominoPreview(const shared_ptr<Tetromino> nextTetromino) {
	const auto& shape = nextTetromino->getShape( );
	int x = nextTetromino->getX( ), y = nextTetromino->getY( );

	double angle = nextTetromino->getRotationAngle( );

	TetrominoShape tetrominoShape = nextTetromino->getShapeEnumn( );

	SDL_SetTextureBlendMode(textures[shapeToAsset(tetrominoShape)], SDL_BLENDMODE_BLEND);
	SDL_Color color = nextTetromino->getColor( );
	SDL_SetTextureColorMod(textures[shapeToAsset(tetrominoShape)], color.r, color.g, color.b);

	if (tetrominoShape == TetrominoShape::I) {
		SDL_SetTextureColorMod(textures[TetrisAssets::I_STARTR], color.r, color.g, color.b);
		SDL_SetTextureColorMod(textures[TetrisAssets::I_MIDR], color.r, color.g, color.b);
		SDL_SetTextureColorMod(textures[TetrisAssets::I_ENDR], color.r, color.g, color.b);

		for (int i = 0; i < 4; ++i) {
			SDL_Rect destRect = { (windowWidth - 155) + (x + i) * blockSize, (windowHeight - 120) + y * blockSize, blockSize, blockSize };
			SDL_RenderCopy(renderer.get( ), textures[TetrisAssets::I_MIDR], nullptr, &destRect);
		}

		SDL_Rect startRect = { (windowWidth - 155) + (x + 0) * blockSize, (windowHeight - 120) + y * blockSize, blockSize, blockSize };
		SDL_RenderCopy(renderer.get( ), textures[TetrisAssets::I_ENDR], nullptr, &startRect);

		SDL_Rect endRect = { (windowWidth - 155) + (x + 3) * blockSize, (windowHeight - 120) + y * blockSize, blockSize, blockSize };
		SDL_RenderCopy(renderer.get( ), textures[TetrisAssets::I_STARTR], nullptr, &endRect);

	} else {
		for (int row = 0; row < shape.size( ); ++row) {
			for (int col = 0; col < shape[row].size( ); ++col) {
				if (shape[row][col] != 0) {
					SDL_Rect destRect = { (windowWidth - 140) + col * blockSize, (windowHeight - 130) + row * blockSize, blockSize, blockSize };
					SDL_Point center = { blockSize / 2, blockSize / 2 };
					SDL_RenderCopy(renderer.get( ), textures[shapeToAsset(tetrominoShape)], nullptr, &destRect);
				}
			}
		}
	}
}

void Renderer::renderText(string text, int x, int y, int size, SDL_Color color) {
	TTF_Font* font = TTF_OpenFont("assets/tetris-gb.ttf", size);
	SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str( ), color);
	if (!surface) {
		SDL_Log("Failed to create surface: %s", TTF_GetError( ));
		return;
	}

	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer.get( ), surface);
	if (!texture) {
		SDL_Log("Failed to create texture from surface: %s", SDL_GetError( ));
		SDL_FreeSurface(surface);
		TTF_CloseFont(font);
		return;
	}

	int width = surface->w;
	int height = surface->h;

	SDL_Rect destRect = { x, y, width, height };

	SDL_RenderCopy(renderer.get( ), texture, nullptr, &destRect);

	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
	TTF_CloseFont(font);
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
