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
	ANGLE_TL, ANGLE_TR, ANGLE_ML, ANGLE_MR, ANGLE_BL, ANGLE_BR,
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

game_sprite SliceSprite(game_sprite* Sprite, void* DestinationMemory, int StartX, int StartY, int Width, int Height);
void ExtractSpriteSheet(void* SourceMemory, void* DestinationMemory, game_sprite* SpriteMap);
