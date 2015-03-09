#pragma once
#include <stdint.h>

//[NOTE] Sprite IDs. NUM_x should always be first. Also the DIGIT_x should be in order.
//       For everything else order should(hopefully) be irrelevant.
enum
{
	NUM_0 = 0, NUM_1, NUM_2, NUM_3, NUM_4, NUM_5, NUM_6, NUM_7, NUM_8,

	BLOCK, BLOCK_PRESSED,

	MINE, FLAG, MINE_WRONG, MINE_RED,

	QUESTION, QUESTION_PRESSED,

	DIGIT_0, DIGIT_1, DIGIT_2, DIGIT_3, DIGIT_4, DIGIT_5, DIGIT_6, DIGIT_7, DIGIT_8, DIGIT_9, DIGIT_NONE,

	FACE_SMILE, FACE_PRESSED, FACE_WIN, FACE_DEAD, FACE_GUESS,

	DIGIT_BOARD,

	//[NOTE] Yeah, it's confusing.
	// T = top, M = middle, B = bottom
	// L = left, R = right
	ANGLE_TL , ANGLE_TR, ANGLE_ML, ANGLE_MR, ANGLE_BL, ANGLE_BR,
	FILLER_T, FILLER_M, FILLER_B,
	FILLER_TL, FILLER_TR,
	FILLER_BL, FILLER_BR,

	COLOR_FILL
};

//[NOTE] I think it is better hardcoding this since adding new sprites
//		 that aren't used yet will cause problems if this got the actual
//		 number of the sprite ids in the enum above.
#define NumberOfSprites 48

struct game_sprite
{
	void* Memory;
	int MemorySize;
	int Width;
	int Height;
};

static game_sprite SliceSprite(game_sprite* Sprite, void* DestinationMemory, int StartX, int StartY, int Width, int Height)
{
	int BytesPerPixel = 4;
	game_sprite Result = {};

	if (StartX + Width <= Sprite->Width && StartY + Height <= Sprite->Height)
	{
		Result.MemorySize = Width*Height*BytesPerPixel;
		Result.Width = Width;
		Result.Height = Height;

		Result.Memory = DestinationMemory;

		uint32_t* ReadPos = (uint32_t*)Sprite->Memory + Sprite->Width*StartY + StartX;
		uint32_t* WritePos = (uint32_t*)Result.Memory;

		for (int Pixel = 0; Pixel < Width*Height; Pixel++)
		{
			if (Pixel % Result.Width == 0 && Pixel != 0)
			{
				ReadPos += Sprite->Width - Result.Width;
			}

			*WritePos = *ReadPos;

			WritePos++;
			ReadPos++;

		}

	}

	return Result;
}

static void ExtractSpriteSheet(void* SourceMemory, void* DestinationMemory, game_sprite* SpriteMap)
{
	game_sprite SpriteSheet = {};

	int32_t* ReadPos = (int32_t*)SourceMemory;

	SpriteSheet.Width = *ReadPos;
	ReadPos++;
	SpriteSheet.Height = *ReadPos;
	ReadPos++;

	SpriteSheet.Memory = ReadPos;

	int BytesPerPixel = 4;
	SpriteSheet.MemorySize = SpriteSheet.Width*SpriteSheet.Height*BytesPerPixel;

	//[NOTE] Not sure if there is a better way than hardcoding the sprite positions in the spritesheet.
	//		 The following just fills the SpriteMap, and advances the DestinationMemory pointer to the next
	//		 position to store the next sprite.memory.

	SpriteMap[NUM_0] = SliceSprite(&SpriteSheet, DestinationMemory, 0 * 16, 0, 16, 16);
	DestinationMemory = (uint8_t*)DestinationMemory + SpriteMap[NUM_0].MemorySize;
	SpriteMap[NUM_1] = SliceSprite(&SpriteSheet, DestinationMemory, 1 * 16, 0, 16, 16);
	DestinationMemory = (uint8_t*)DestinationMemory + SpriteMap[NUM_1].MemorySize;
	SpriteMap[NUM_2] = SliceSprite(&SpriteSheet, DestinationMemory, 2 * 16, 0, 16, 16);
	DestinationMemory = (uint8_t*)DestinationMemory + SpriteMap[NUM_2].MemorySize;
	SpriteMap[NUM_3] = SliceSprite(&SpriteSheet, DestinationMemory, 3 * 16, 0, 16, 16);
	DestinationMemory = (uint8_t*)DestinationMemory + SpriteMap[NUM_3].MemorySize;
	SpriteMap[NUM_4] = SliceSprite(&SpriteSheet, DestinationMemory, 4 * 16, 0, 16, 16);
	DestinationMemory = (uint8_t*)DestinationMemory + SpriteMap[NUM_4].MemorySize;
	SpriteMap[NUM_5] = SliceSprite(&SpriteSheet, DestinationMemory, 5 * 16, 0, 16, 16);
	DestinationMemory = (uint8_t*)DestinationMemory + SpriteMap[NUM_5].MemorySize;
	SpriteMap[NUM_6] = SliceSprite(&SpriteSheet, DestinationMemory, 6 * 16, 0, 16, 16);
	DestinationMemory = (uint8_t*)DestinationMemory + SpriteMap[NUM_6].MemorySize;
	SpriteMap[NUM_7] = SliceSprite(&SpriteSheet, DestinationMemory, 7 * 16, 0, 16, 16);
	DestinationMemory = (uint8_t*)DestinationMemory + SpriteMap[NUM_7].MemorySize;
	SpriteMap[NUM_8] = SliceSprite(&SpriteSheet, DestinationMemory, 8 * 16, 0, 16, 16);
	DestinationMemory = (uint8_t*)DestinationMemory + SpriteMap[NUM_8].MemorySize;

	SpriteMap[BLOCK] = SliceSprite(&SpriteSheet, DestinationMemory, 0 * 16, 16, 16, 16);
	DestinationMemory = (uint8_t*)DestinationMemory + SpriteMap[BLOCK].MemorySize;
	SpriteMap[BLOCK_PRESSED] = SliceSprite(&SpriteSheet, DestinationMemory, 1 * 16, 16, 16, 16);
	DestinationMemory = (uint8_t*)DestinationMemory + SpriteMap[BLOCK_PRESSED].MemorySize;
	SpriteMap[MINE] = SliceSprite(&SpriteSheet, DestinationMemory, 2 * 16, 16, 16, 16);
	DestinationMemory = (uint8_t*)DestinationMemory + SpriteMap[MINE].MemorySize;
	SpriteMap[FLAG] = SliceSprite(&SpriteSheet, DestinationMemory, 3 * 16, 16, 16, 16);
	DestinationMemory = (uint8_t*)DestinationMemory + SpriteMap[FLAG].MemorySize;
	SpriteMap[MINE_WRONG] = SliceSprite(&SpriteSheet, DestinationMemory, 4 * 16, 16, 16, 16);
	DestinationMemory = (uint8_t*)DestinationMemory + SpriteMap[MINE_WRONG].MemorySize;
	SpriteMap[MINE_RED] = SliceSprite(&SpriteSheet, DestinationMemory, 5 * 16, 16, 16, 16);
	DestinationMemory = (uint8_t*)DestinationMemory + SpriteMap[MINE_RED].MemorySize;
	SpriteMap[QUESTION] = SliceSprite(&SpriteSheet, DestinationMemory, 6 * 16, 16, 16, 16);
	DestinationMemory = (uint8_t*)DestinationMemory + SpriteMap[QUESTION].MemorySize;
	SpriteMap[QUESTION_PRESSED] = SliceSprite(&SpriteSheet, DestinationMemory, 7 * 16, 16, 16, 16);
	DestinationMemory = (uint8_t*)DestinationMemory + SpriteMap[QUESTION_PRESSED].MemorySize;

	SpriteMap[DIGIT_0] = SliceSprite(&SpriteSheet, DestinationMemory, 0 * 12, 33, 11, 21);
	DestinationMemory = (uint8_t*)DestinationMemory + SpriteMap[DIGIT_0].MemorySize;
	SpriteMap[DIGIT_1] = SliceSprite(&SpriteSheet, DestinationMemory, 1 * 12, 33, 11, 21);
	DestinationMemory = (uint8_t*)DestinationMemory + SpriteMap[DIGIT_1].MemorySize;
	SpriteMap[DIGIT_2] = SliceSprite(&SpriteSheet, DestinationMemory, 2 * 12, 33, 11, 21);
	DestinationMemory = (uint8_t*)DestinationMemory + SpriteMap[DIGIT_2].MemorySize;
	SpriteMap[DIGIT_3] = SliceSprite(&SpriteSheet, DestinationMemory, 3 * 12, 33, 11, 21);
	DestinationMemory = (uint8_t*)DestinationMemory + SpriteMap[DIGIT_3].MemorySize;
	SpriteMap[DIGIT_4] = SliceSprite(&SpriteSheet, DestinationMemory, 4 * 12, 33, 11, 21);
	DestinationMemory = (uint8_t*)DestinationMemory + SpriteMap[DIGIT_4].MemorySize;
	SpriteMap[DIGIT_5] = SliceSprite(&SpriteSheet, DestinationMemory, 5 * 12, 33, 11, 21);;
	DestinationMemory = (uint8_t*)DestinationMemory + SpriteMap[DIGIT_5].MemorySize;
	SpriteMap[DIGIT_6] = SliceSprite(&SpriteSheet, DestinationMemory, 6 * 12, 33, 11, 21);
	DestinationMemory = (uint8_t*)DestinationMemory + SpriteMap[DIGIT_6].MemorySize;
	SpriteMap[DIGIT_7] = SliceSprite(&SpriteSheet, DestinationMemory, 7 * 12, 33, 11, 21);
	DestinationMemory = (uint8_t*)DestinationMemory + SpriteMap[DIGIT_7].MemorySize;
	SpriteMap[DIGIT_8] = SliceSprite(&SpriteSheet, DestinationMemory, 8 * 12, 33, 11, 21);
	DestinationMemory = (uint8_t*)DestinationMemory + SpriteMap[DIGIT_8].MemorySize;
	SpriteMap[DIGIT_9] = SliceSprite(&SpriteSheet, DestinationMemory, 9 * 12, 33, 11, 21);
	DestinationMemory = (uint8_t*)DestinationMemory + SpriteMap[DIGIT_9].MemorySize;
	SpriteMap[DIGIT_NONE] = SliceSprite(&SpriteSheet, DestinationMemory, 10 * 12, 33, 11, 21);
	DestinationMemory = (uint8_t*)DestinationMemory + SpriteMap[DIGIT_NONE].MemorySize;

	SpriteMap[FACE_SMILE] = SliceSprite(&SpriteSheet, DestinationMemory, 0*27, 55, 26, 26);
	DestinationMemory = (uint8_t*)DestinationMemory + SpriteMap[FACE_SMILE].MemorySize;
	SpriteMap[FACE_GUESS] = SliceSprite(&SpriteSheet, DestinationMemory, 1*27, 55, 26, 26);
	DestinationMemory = (uint8_t*)DestinationMemory + SpriteMap[FACE_GUESS].MemorySize;
	SpriteMap[FACE_DEAD] = SliceSprite(&SpriteSheet, DestinationMemory, 2*27, 55, 26, 26);
	DestinationMemory = (uint8_t*)DestinationMemory + SpriteMap[FACE_DEAD].MemorySize;
	SpriteMap[FACE_WIN] = SliceSprite(&SpriteSheet, DestinationMemory, 3*27, 55, 26, 26);
	DestinationMemory = (uint8_t*)DestinationMemory + SpriteMap[FACE_WIN].MemorySize;
	SpriteMap[FACE_PRESSED] = SliceSprite(&SpriteSheet, DestinationMemory, 4*27, 55, 26, 26);
	DestinationMemory = (uint8_t*)DestinationMemory + SpriteMap[FACE_PRESSED].MemorySize;

	SpriteMap[DIGIT_BOARD] = SliceSprite(&SpriteSheet, DestinationMemory, 28, 82, 41, 25);
	DestinationMemory = (uint8_t*)DestinationMemory + SpriteMap[DIGIT_BOARD].MemorySize;

	SpriteMap[ANGLE_TL] = SliceSprite(&SpriteSheet, DestinationMemory, 0, 82, 12, 11);
	DestinationMemory = (uint8_t*)DestinationMemory + SpriteMap[ANGLE_TL].MemorySize;
	SpriteMap[ANGLE_TR] = SliceSprite(&SpriteSheet, DestinationMemory, 15, 82, 12, 11);
	DestinationMemory = (uint8_t*)DestinationMemory + SpriteMap[ANGLE_TR].MemorySize;
	SpriteMap[ANGLE_ML] = SliceSprite(&SpriteSheet, DestinationMemory, 0, 96, 12, 11);
	DestinationMemory = (uint8_t*)DestinationMemory + SpriteMap[ANGLE_ML].MemorySize;
	SpriteMap[ANGLE_MR] = SliceSprite(&SpriteSheet, DestinationMemory, 15, 96, 12, 11);
	DestinationMemory = (uint8_t*)DestinationMemory + SpriteMap[ANGLE_MR].MemorySize;
	SpriteMap[ANGLE_BL] = SliceSprite(&SpriteSheet, DestinationMemory, 0, 110, 12, 11);
	DestinationMemory = (uint8_t*)DestinationMemory + SpriteMap[ANGLE_BL].MemorySize;
	SpriteMap[ANGLE_BR] = SliceSprite(&SpriteSheet, DestinationMemory, 15, 110, 12, 11);
	DestinationMemory = (uint8_t*)DestinationMemory + SpriteMap[ANGLE_BR].MemorySize;

	SpriteMap[FILLER_T] = SliceSprite(&SpriteSheet, DestinationMemory, 13, 82, 1, 11);
	DestinationMemory = (uint8_t*)DestinationMemory + SpriteMap[FILLER_T].MemorySize;
	SpriteMap[FILLER_M] = SliceSprite(&SpriteSheet, DestinationMemory, 13, 96, 1, 11);
	DestinationMemory = (uint8_t*)DestinationMemory + SpriteMap[FILLER_M].MemorySize;
	SpriteMap[FILLER_B] = SliceSprite(&SpriteSheet, DestinationMemory, 13, 110, 1, 11);
	DestinationMemory = (uint8_t*)DestinationMemory + SpriteMap[FILLER_B].MemorySize;

	SpriteMap[FILLER_TL] = SliceSprite(&SpriteSheet, DestinationMemory, 0, 94, 12, 1);
	DestinationMemory = (uint8_t*)DestinationMemory + SpriteMap[FILLER_TL].MemorySize;
	SpriteMap[FILLER_TR] = SliceSprite(&SpriteSheet, DestinationMemory, 15, 94, 12, 1);
	DestinationMemory = (uint8_t*)DestinationMemory + SpriteMap[FILLER_TR].MemorySize;
	SpriteMap[FILLER_BL] = SliceSprite(&SpriteSheet, DestinationMemory, 0, 108, 12, 1);
	DestinationMemory = (uint8_t*)DestinationMemory + SpriteMap[FILLER_BL].MemorySize;
	SpriteMap[FILLER_BR] = SliceSprite(&SpriteSheet, DestinationMemory, 15, 108, 12, 1);
	DestinationMemory = (uint8_t*)DestinationMemory + SpriteMap[FILLER_BR].MemorySize;

	SpriteMap[COLOR_FILL] = SliceSprite(&SpriteSheet, DestinationMemory, 70, 82, 1, 1);
}