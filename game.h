#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include "graphics.h"
struct game_raw_input
{
	int MouseX;
	int MouseY;

	bool LeftMouseDown;
	bool LeftMouseUp;

	bool RightMouseDown;
	bool RightMouseUp;

	enum { KEYB_OTHER, KEYB_UP, KEYB_DOWN, KEYB_LEFT, KEYB_RIGHT, KEYB_PLUS, KEYB_MINUS, KEYB_SUGGEST } KeyUp;
};


struct game_input
{
	int MouseX;
	int MouseY;

	enum { MB_NONE, MB_LEFT_UP, MB_LEFT_DOWN, MB_RIGHT_UP, MB_RIGHT_DOWN, MB_BOTH_DOWN, MB_BOTH_UP } MouseControl;

	bool WasKeyboardInput;

	int HeightModifier;
	int WidthModifier;
	int MineModifier;
	bool SuggestMines;
};

struct game_memory
{
	bool IsInitialized;

	//[NOTE] This stores the whole spritesheet at first
	//       and then the game board. Any leftover memory
	//       is used as a stack for the flood fill.
	void* TransientStorage;
	int TransientStorageSize;

	//[NOTE] This stores the sprites
	void* PermanentStorage;
	int PermanentStorageSize;
};

struct game_board_data
{
	uint8_t Contents;
	enum { NONE, NONE_DOWN, FLAG, QUESTION, QUESTION_DOWN, REVEALED } State;
};

struct game_board
{
	game_board_data *Tile;
	int Width;
	int Height;
};

struct game_time
{
	bool HasStarted;
	clock_t StartTime;
	clock_t EndTime;
	uint32_t SecondsElapsed;
};

struct game_options
{
	int BoardX;
	int BoardY;
	int Mines;
};


void GameUpdateAndRender(game_memory* GameMemory, game_buffer* BackBuffer, game_raw_input* PreviousRawInput, game_raw_input* CurrentRawInput);
