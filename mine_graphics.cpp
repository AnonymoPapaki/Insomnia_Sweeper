#pragma once
#include <stdlib.h>
#include <stdint.h>
#include "helper.h"
#include "mine_spritework.cpp"

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

static game_ui UI;

static void DrawSpriteOnBuffer(game_buffer* BackBuffer, game_sprite* Sprite, int DrawX, int DrawY)
{
	uint32_t* WritePos = (uint32_t*)(BackBuffer->Memory);
	WritePos = WritePos + DrawY*BackBuffer->Width + DrawX;
	uint32_t* ReadPos = (uint32_t*)Sprite->Memory;

	//[TODO] Doesn't check for overflow
	for (int Row = 0; Row < Sprite->Height; Row++)
	{
		for (int Column = 0; Column < Sprite->Width; Column++)
		{
			*WritePos = *ReadPos;

			WritePos++;
			ReadPos++;
		}

		WritePos = WritePos + BackBuffer->Width - Sprite->Width;
	}

}

static void TrailSpriteOnBuffer(game_buffer* BackBuffer, game_sprite* Sprite, int DrawX, int DrawY, int XCopies, int YCopies)
{
	for (int Trail = 0; Trail <= XCopies; Trail++)
	{
		DrawSpriteOnBuffer(BackBuffer, Sprite, DrawX + Trail*Sprite->Width, DrawY);
	}

	for (int Trail = 0; Trail <= YCopies; Trail++)
	{
		DrawSpriteOnBuffer(BackBuffer, Sprite, DrawX, DrawY + Trail*Sprite->Height);
	}
}

static void InitializeGameUI(int XTiles, int YTiles, game_sprite* SpriteMap)
{
	UI.XTiles = XTiles;
	UI.YTiles = YTiles;

	UI.OffsetTop = 4;
	UI.OffsetBottom = 4;
	UI.OffsetLeft = 4;
	UI.OffsetRight = 4;

	UI.BlockWidth = SpriteMap[BLOCK].Width;
	UI.BlockHeight = SpriteMap[BLOCK].Height;

	UI.BorderHeight = SpriteMap[ANGLE_TL].Height;
	UI.BorderWidth = SpriteMap[ANGLE_TL].Width;

	UI.HorizontalBorderLength = UI.BlockWidth * UI.XTiles;
	UI.VerticalUpperBorderLength = UI.OffsetTop + UI.OffsetBottom + SpriteMap[DIGIT_BOARD].Height;
	UI.VerticalLowerBorderLength = UI.BlockHeight * UI.YTiles;

	UI.UpperSpaceHeight = UI.VerticalUpperBorderLength + 2 * UI.BorderHeight;

	UI.GameWidth = UI.HorizontalBorderLength + 2 * UI.BorderWidth;
	UI.GameHeight = UI.VerticalLowerBorderLength + 3 * UI.BorderHeight + UI.VerticalUpperBorderLength;

	UI.DigitWidth = SpriteMap[DIGIT_0].Width;
	UI.DigitBoardWidth = SpriteMap[DIGIT_BOARD].Width;
	UI.DigitBoardLeftX = UI.BorderWidth + UI.OffsetLeft;
	UI.DigitBoardRightX = UI.GameWidth - UI.BorderWidth - UI.DigitBoardWidth - UI.OffsetRight;
	UI.DigitBoardY = UI.BorderHeight + UI.OffsetTop;

	UI.FaceWidth = SpriteMap[FACE_SMILE].Width;
	UI.FaceHeight = SpriteMap[FACE_SMILE].Height;
	UI.FaceX = (UI.GameWidth - UI.FaceWidth) / 2;
	UI.FaceY = UI.BorderHeight + UI.OffsetTop;
}

static void DrawBoardOutline(game_buffer* BackBuffer, game_sprite* SpriteMap)
{
	TrailSpriteOnBuffer(BackBuffer, &SpriteMap[FILLER_T], UI.BorderWidth, 0, UI.HorizontalBorderLength, 0);

	TrailSpriteOnBuffer(BackBuffer, &SpriteMap[FILLER_TL], 0, UI.BorderHeight, 0, UI.VerticalUpperBorderLength);
	TrailSpriteOnBuffer(BackBuffer, &SpriteMap[FILLER_TR], UI.GameWidth - UI.BorderWidth, UI.BorderHeight, 0, UI.VerticalUpperBorderLength);

	DrawSpriteOnBuffer(BackBuffer, &SpriteMap[ANGLE_TL], 0, 0);
	DrawSpriteOnBuffer(BackBuffer, &SpriteMap[ANGLE_TR], UI.GameWidth - UI.BorderWidth, 0);

	DrawSpriteOnBuffer(BackBuffer, &SpriteMap[ANGLE_ML], 0, UI.UpperSpaceHeight - UI.BorderHeight);
	DrawSpriteOnBuffer(BackBuffer, &SpriteMap[ANGLE_MR], UI.GameWidth - UI.BorderWidth, UI.UpperSpaceHeight - UI.BorderHeight);

	TrailSpriteOnBuffer(BackBuffer, &SpriteMap[FILLER_M], UI.BorderWidth, UI.UpperSpaceHeight - UI.BorderHeight, UI.HorizontalBorderLength, 0);

	TrailSpriteOnBuffer(BackBuffer, &SpriteMap[FILLER_BL], 0, UI.UpperSpaceHeight, 0, UI.VerticalLowerBorderLength);
	TrailSpriteOnBuffer(BackBuffer, &SpriteMap[FILLER_BR], UI.GameWidth - UI.BorderWidth, UI.UpperSpaceHeight, 0, UI.VerticalLowerBorderLength);

	DrawSpriteOnBuffer(BackBuffer, &SpriteMap[ANGLE_BL], 0, UI.GameHeight - UI.BorderHeight);
	DrawSpriteOnBuffer(BackBuffer, &SpriteMap[ANGLE_BR], UI.GameWidth - UI.BorderWidth, UI.GameHeight - UI.BorderHeight);

	TrailSpriteOnBuffer(BackBuffer, &SpriteMap[FILLER_B], UI.BorderWidth, UI.GameHeight - UI.BorderHeight, UI.HorizontalBorderLength, 0);

	for (int FillLines = 0; FillLines < UI.VerticalUpperBorderLength; FillLines++)
	{
		TrailSpriteOnBuffer(BackBuffer, &SpriteMap[COLOR_FILL], UI.BorderWidth, UI.BorderWidth + FillLines - 1, UI.HorizontalBorderLength, 0);
	}

	DrawSpriteOnBuffer(BackBuffer, &SpriteMap[DIGIT_BOARD], UI.DigitBoardLeftX, UI.DigitBoardY);
	DrawSpriteOnBuffer(BackBuffer, &SpriteMap[DIGIT_BOARD], UI.DigitBoardRightX, UI.DigitBoardY);

	DrawSpriteOnBuffer(BackBuffer, &SpriteMap[FACE_SMILE], UI.FaceX, UI.FaceY);
}

static void InitializeGame(game_buffer* BackBuffer, int BoardX, int BoardY, game_sprite* SpriteMap)
{
	InitializeGameUI(BoardX, BoardY, SpriteMap);

	//[NOTE] Board sizes for a WxH game:
	// width	= 16*W + 2*12				= 16*W + 24
	// height	= 16*H + 3*11 + 25 + 2*4	= 16*H + 66
	//
	// so for the easy 9x9 game, we need a 168x210 buffer to draw on.
	BackBuffer->Width = UI.BlockWidth * BoardX + 2 * UI.BorderWidth;
	BackBuffer->Height = UI.BlockHeight * BoardY + 3 * UI.BorderHeight + UI.VerticalUpperBorderLength;


	DrawBoardOutline(BackBuffer, SpriteMap);
}