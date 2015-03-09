#pragma once

#include "spritework.h"

struct game_buffer
{
	void* Memory;
	int MemorySize;
	int Width;
	int Height;
};

struct game_ui
{
	int XTiles;
	int YTiles;

	int OffsetTop;
	int OffsetBottom;
	int OffsetLeft;
	int OffsetRight;

	int BorderHeight;
	int BorderWidth;

	int BlockWidth;
	int BlockHeight;

	int HorizontalBorderLength;
	int VerticalUpperBorderLength;
	int VerticalLowerBorderLength;
	int UpperSpaceHeight;

	int GameWidth;
	int GameHeight;

	int DigitWidth;
	int DigitBoardWidth;
	int DigitBoardLeftX;
	int DigitBoardRightX;
	int DigitBoardY;

	int FaceX;
	int FaceY;
	int FaceWidth;
	int FaceHeight;
};


extern game_ui UI;

void DrawSpriteOnBuffer(game_buffer* BackBuffer, game_sprite* Sprite, int DrawX, int DrawY);
void TrailSpriteOnBuffer(game_buffer* BackBuffer, game_sprite* Sprite, int DrawX, int DrawY, int XCopies, int YCopies);
void InitializeGameUI(int XTiles, int YTiles, game_sprite* SpriteMap);
void DrawBoardOutline(game_buffer* BackBuffer, game_sprite* SpriteMap);
void InitializeGame(game_buffer* BackBuffer, int BoardX, int BoardY, game_sprite* SpriteMap);