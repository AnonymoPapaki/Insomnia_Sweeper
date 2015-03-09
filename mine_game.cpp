#pragma once

#include "game.h"
#include "graphics.h"
#include "helper.h"



static game_sprite SpriteMap[NumberOfSprites];
static game_board Board;
static game_time GameTime;
static game_options GameOptions;
static enum { GS_RUNNING, GS_NEW, GS_LOSE, GS_WIN } GameState;


static enum { FC_NORMAL, FC_DOWN, FC_GUESS } FaceState;
static int NotFlaggedMines;
static int TilesToReveal;

inline void DrawNumberOnDigitBoard(game_buffer* BackBuffer, int Number, int DigitBoardXOffset)
{
	//[NOTE] Adjust Number to fix the digit board.
	int AdjustedNumber = (Number > 999) ? 999 :
						(Number < -99) ? -99 : Number;

	int FirstDigit = (AdjustedNumber < 0) ? DIGIT_NONE : DIGIT_0 + (AdjustedNumber / 100);
	int SecondDigit = DIGIT_0 + ((Absolute(AdjustedNumber) / 10) % 10);
	int ThirdDigit = DIGIT_0 + (Absolute(AdjustedNumber) % 10);

	DrawSpriteOnBuffer(BackBuffer, &SpriteMap[FirstDigit], DigitBoardXOffset + 2, UI.DigitBoardY + 2);
	DrawSpriteOnBuffer(BackBuffer, &SpriteMap[SecondDigit], DigitBoardXOffset + 2 + UI.DigitWidth + 2, UI.DigitBoardY + 2);
	DrawSpriteOnBuffer(BackBuffer, &SpriteMap[ThirdDigit], DigitBoardXOffset + 2 + 2 * UI.DigitWidth + 4, UI.DigitBoardY + 2);

}

static void DrawActiveSprites(game_buffer* BackBuffer)
{
	for (int TileY = 0; TileY < UI.YTiles; TileY++)
		for (int TileX = 0; TileX < UI.XTiles; TileX++)
		{
			int X = UI.BorderWidth + TileX*SpriteMap[BLOCK].Width;
			int Y = UI.UpperSpaceHeight + TileY*SpriteMap[BLOCK].Height;

			int Position = TileY*Board.Width + TileX;

			if (Board.Tile[Position].State == Board.Tile->NONE)
			{
				if (GameState == GS_LOSE)
				{
					uint8_t TileContents = Board.Tile[Position].Contents;
					DrawSpriteOnBuffer(BackBuffer, &SpriteMap[TileContents], X, Y);
				}
				else if (GameState == GS_WIN)
				{
					//[NOTE] If the game is won then the !REVEALED tiles are cetain to
					//       be mines, since all the rest are revealed.
					DrawSpriteOnBuffer(BackBuffer, &SpriteMap[FLAG], X, Y);
				}
				else
				{
					DrawSpriteOnBuffer(BackBuffer, &SpriteMap[BLOCK], X, Y);
				}

			}
			else if (Board.Tile[Position].State == Board.Tile->QUESTION)
			{
				if (GameState == GS_LOSE)
				{
					uint8_t TileContents = Board.Tile[Position].Contents;
					DrawSpriteOnBuffer(BackBuffer, &SpriteMap[TileContents], X, Y);
				}
				else if (GameState == GS_WIN)
				{
					//[NOTE] If the game is won then the !REVEALED tiles are cetain to
					//       be mines, since all the rest are revealed.
					DrawSpriteOnBuffer(BackBuffer, &SpriteMap[FLAG], X, Y);
				}
				else
				{
					DrawSpriteOnBuffer(BackBuffer, &SpriteMap[QUESTION], X, Y);
				}
			}
			else if (Board.Tile[Position].State == Board.Tile->NONE_DOWN)
			{
				DrawSpriteOnBuffer(BackBuffer, &SpriteMap[BLOCK_PRESSED], X, Y);
				Board.Tile[Position].State = Board.Tile->NONE;
			}
			else if (Board.Tile[Position].State == Board.Tile->QUESTION_DOWN)
			{
				DrawSpriteOnBuffer(BackBuffer, &SpriteMap[QUESTION_PRESSED], X, Y);
				Board.Tile[Position].State = Board.Tile->QUESTION;
			}
			else if (Board.Tile[Position].State == Board.Tile->FLAG)
			{
				if (GameState == GS_LOSE && Board.Tile[Position].Contents != MINE)
				{
					DrawSpriteOnBuffer(BackBuffer, &SpriteMap[MINE_WRONG], X, Y);
				}
				else
				{
					DrawSpriteOnBuffer(BackBuffer, &SpriteMap[FLAG], X, Y);
				}
			}
			else if (Board.Tile[Position].State == Board.Tile->REVEALED)
			{
				uint8_t TileContents = Board.Tile[Position].Contents;
				DrawSpriteOnBuffer(BackBuffer, &SpriteMap[TileContents], X, Y);
			}

		}

	int FaceSpriteID = (FaceState == FC_DOWN) ? FACE_PRESSED :
						(GameState == GS_WIN) ? FACE_WIN :
						(GameState == GS_LOSE) ? FACE_DEAD :
						(FaceState == FC_GUESS) ? FACE_GUESS :
						FACE_SMILE;
	DrawSpriteOnBuffer(BackBuffer, &SpriteMap[FaceSpriteID], UI.FaceX, UI.FaceY);
	FaceState = FC_NORMAL;

	DrawNumberOnDigitBoard(BackBuffer, NotFlaggedMines, UI.DigitBoardLeftX);
	DrawNumberOnDigitBoard(BackBuffer, GameTime.SecondsElapsed, UI.DigitBoardRightX);
}



static void FillGameBoard(void* Memory, int Width, int Height, int NumberOfMines)
{
	NotFlaggedMines = NumberOfMines;
	TilesToReveal = Width*Height - NumberOfMines;

	Board.Tile = (game_board_data *)Memory;
	Board.Width = Width;
	Board.Height = Height;

	game_board_data* WritePos = (game_board_data *)Memory;

	for (int Index = 0; Index < Board.Width*Board.Height; Index++)
	{ 
		game_board_data Data;
		Data.Contents = 0;
		Data.State = Data.NONE;

		*WritePos = Data;

		WritePos++;
	}

	srand((uint32_t)time(0));
	for (int Mine = 0; Mine < NumberOfMines; Mine++)
	{
		int MinePosition = rand() % (Board.Width*Board.Height);

		if (Board.Tile[MinePosition].Contents == MINE)
		{
			Mine--;
			continue;
		}

		Board.Tile[MinePosition].Contents = MINE;

		//[NOTE] Increment empty neighbor tiles to the mine
		for (int XOffset = -1; XOffset <= 1; XOffset++)
			for (int YOffset = -1; YOffset <= 1; YOffset++)
			{
				int MineY = MinePosition / Board.Width;
				int MineX = MinePosition - MineY*Board.Width;

				if ((MineX + XOffset) < UI.XTiles && (MineY + YOffset) < UI.YTiles &&
					(MineX + XOffset) >= 0 && (MineY + YOffset) >= 0)
				{
					int Neighbor = MinePosition + YOffset*Board.Width + XOffset;

					if (Board.Tile[Neighbor].Contents != MINE)
					{
						Board.Tile[Neighbor].Contents++;
					}
				}
			}
	}

}


static game_input ProcessRawInput(game_raw_input* PreviousRawInput, game_raw_input* CurrentRawInput)
{
	game_input Result = {};

	//[NOTE] Keyboard input
	Result.WasKeyboardInput = true;
	Result.SuggestMines = false;
	if (CurrentRawInput->KeyUp == CurrentRawInput->KEYB_UP)
	{
		Result.HeightModifier = -1;
	}
	else if (CurrentRawInput->KeyUp == CurrentRawInput->KEYB_DOWN)
	{
		Result.HeightModifier = 1;
	}
	else if (CurrentRawInput->KeyUp == CurrentRawInput->KEYB_RIGHT)
	{
		Result.WidthModifier = 1;
	}
	else if (CurrentRawInput->KeyUp == CurrentRawInput->KEYB_LEFT)
	{
		Result.WidthModifier = -1;
	}
	else if (CurrentRawInput->KeyUp == CurrentRawInput->KEYB_PLUS)
	{
		Result.MineModifier = 1;
	}
	else if (CurrentRawInput->KeyUp == CurrentRawInput->KEYB_MINUS)
	{
		Result.MineModifier = -1;
	}
	else if (CurrentRawInput->KeyUp == CurrentRawInput->KEYB_SUGGEST)
	{
		Result.SuggestMines = true;
	}
	else
	{
		Result.WasKeyboardInput = false;
	}


	//[NOTE] Mouse input
	Result.MouseX = CurrentRawInput->MouseX;
	Result.MouseY = CurrentRawInput->MouseY;

	if ((PreviousRawInput->LeftMouseDown && PreviousRawInput->RightMouseDown) &&
		(!CurrentRawInput->LeftMouseDown || !CurrentRawInput->RightMouseDown))
	{
		//[NOTE] This kind of input comes after a user stops pressing both mouse keys.
		Result.MouseControl = Result.MB_BOTH_UP;
	}
	else if (CurrentRawInput->LeftMouseDown && CurrentRawInput->RightMouseDown)
	{
		Result.MouseControl = Result.MB_BOTH_DOWN;
	}
	else if (CurrentRawInput->LeftMouseDown)
	{
		Result.MouseControl = Result.MB_LEFT_DOWN;
	}
	else if (CurrentRawInput->LeftMouseUp && PreviousRawInput->LeftMouseDown)
	{
		Result.MouseControl = Result.MB_LEFT_UP;
	}
	else if (CurrentRawInput->RightMouseDown)
	{
		Result.MouseControl = Result.MB_RIGHT_DOWN;
	}
	else if (CurrentRawInput->RightMouseUp && PreviousRawInput->RightMouseDown)
	{
		Result.MouseControl = Result.MB_RIGHT_UP;
	}
	else
	{
		Result.MouseControl = Result.MB_NONE;
	}

	return Result;
}

static void FloodFillEmptyNeighbors(int Position, void* StackMemory)
{
	int* WritePos = (int*)StackMemory;

	*WritePos = Position;

	while (WritePos >= StackMemory)
	{
		Position = *WritePos;

		int PickedNeighbor = -1;

		for (int XOffset = -1; XOffset <= 1; XOffset++)
			for (int YOffset = -1; YOffset <= 1; YOffset++)
			{
				int PosY = Position / Board.Width;
				int PosX = Position - PosY*Board.Width;

				if ((PosX + XOffset) < UI.XTiles && (PosY + YOffset) < UI.YTiles &&
					(PosX + XOffset) >= 0 && (PosY + YOffset) >= 0)
				{
					int Neighbor = Position + YOffset*Board.Width + XOffset;

					if (Board.Tile[Neighbor].State != Board.Tile->REVEALED &&
						Board.Tile[Neighbor].State != Board.Tile->FLAG)
					{
						Board.Tile[Neighbor].State = Board.Tile->REVEALED;
						TilesToReveal--;

						if (Board.Tile[Neighbor].Contents == 0)
						{
							PickedNeighbor = Neighbor;
							goto LoopEnd;
						}
					}
				}
			}

	LoopEnd:

		if (PickedNeighbor == -1)
		{
			WritePos--;
		}
		else
		{
			WritePos++;
			*WritePos = PickedNeighbor;
		}

	}

}

static void HandleGameInput(game_input* GameInput, game_buffer* BackBuffer, game_memory* GameMemory)
{
	//[NOTE] If there is some mouse input in the upper part of the game
	if (GameInput->MouseControl != GameInput->MB_NONE &&
		GameInput->MouseY < UI.UpperSpaceHeight)
	{
		if (GameInput->MouseX >= UI.FaceX && GameInput->MouseX < (UI.FaceX + UI.FaceWidth) &&
			GameInput->MouseY >= UI.FaceY && GameInput->MouseY < (UI.FaceY + UI.FaceHeight))
		{
			if (GameInput->MouseControl == GameInput->MB_LEFT_DOWN)
			{
				FaceState = FC_DOWN;
			}
			else if (GameInput->MouseControl == GameInput->MB_LEFT_UP)
			{
				GameState = GS_NEW;
			}
		}

	}

	//[NOTE] Handling of keyboard actions
	else if (GameState != GS_NEW &&
		GameInput->WasKeyboardInput)
	{
		//[TODO] The maximum board size should be defined somewhere
		//		 so that it allows the program to allocate its memory
		//		 based on that when it loads.
		int MinimumBoardX = 8;
		int MaximumBoardX = 100;
		int MinimumBoardY = 1;
		int MaximumBoardY = 100;
		int newBoardX = GameOptions.BoardX + GameInput->WidthModifier;
		if (newBoardX >= MinimumBoardX && newBoardX <= MaximumBoardX)
		{
			GameOptions.BoardX += GameInput->WidthModifier;
		}

		int newBoardY = GameOptions.BoardY + GameInput->HeightModifier;
		if (newBoardY >= MinimumBoardY && newBoardY <= MaximumBoardY)
		{
			GameOptions.BoardY += GameInput->HeightModifier;
		}

		int maxMines = GameOptions.BoardX*GameOptions.BoardY;
		if (GameOptions.Mines > maxMines)
		{
			GameOptions.Mines = maxMines - 1;
		}

		if (GameInput->SuggestMines)
		{
			GameOptions.Mines = (int)(0.13f * (GameOptions.BoardX * GameOptions.BoardY));
		}
		else
		{
			int newMines = GameOptions.Mines + GameInput->MineModifier;
			if (newMines >= 1 && newMines < maxMines)
			{
				GameOptions.Mines += GameInput->MineModifier;
			}
		}

		InitializeGame(BackBuffer, GameOptions.BoardX, GameOptions.BoardY, SpriteMap);

		//[NOTE] New game on change style.
		GameState = GS_NEW;
	} 

	//[NOTE] Handling mouse input when the game is running
	else if (GameState == GS_RUNNING &&
		GameInput->MouseControl != GameInput->MB_NONE &&
		//[NOTE] It may seem that the following two checks are not necessary
		//       but the integer division below to find the clicked tile will
		//       fail to identify some negative values (-0.9 == 0 >= 0!!) and
		//       so some areas outside of the board will be found valid.
		GameInput->MouseX >= UI.BorderWidth &&
		GameInput->MouseY >= UI.UpperSpaceHeight)
	{
		if (!GameTime.HasStarted)
		{
			GameTime.StartTime = clock();
			GameTime.HasStarted = true;
		}

		int PressedX = (GameInput->MouseX - UI.BorderWidth) / UI.BlockWidth;
		int PressedY = (GameInput->MouseY - UI.UpperSpaceHeight) / UI.BlockHeight;
		int PressedTile = PressedY*Board.Width + PressedX;

		if (PressedX < UI.XTiles && PressedY < UI.YTiles && PressedX >= 0 && PressedY >= 0)
		{
			if (GameInput->MouseControl == GameInput->MB_BOTH_DOWN)
			{
				for (int XOffset = -1; XOffset <= 1; XOffset++)
					for (int YOffset = -1; YOffset <= 1; YOffset++)
					{
						int Neighbor = PressedTile + YOffset*Board.Width + XOffset;

						if ((PressedX + XOffset) < UI.XTiles && (PressedY + YOffset) < UI.YTiles &&
							(PressedX + XOffset) >= 0 && (PressedY + YOffset) >= 0)
						{
							if (Board.Tile[Neighbor].State == Board.Tile->NONE)
							{
								Board.Tile[Neighbor].State = Board.Tile->NONE_DOWN;
							}
							else if (Board.Tile[Neighbor].State == Board.Tile->QUESTION)
							{
								Board.Tile[Neighbor].State = Board.Tile->QUESTION_DOWN;
							}
						}
					}

				FaceState = FC_GUESS;
			}
			else if (GameInput->MouseControl == GameInput->MB_BOTH_UP)
			{
				if (Board.Tile[PressedTile].State == Board.Tile->REVEALED)
				{
					int TileContents = Board.Tile[PressedTile].Contents;

					//[NOTE] It's impossible to be here and a mine
					//       is in the TileContents.
					//       Also revealed zeros should have all 
					//       tiles around them revealed too, so this
					//       check is not that necessary.
					if (TileContents != NUM_0)
					{
						int FlaggedNeighbors = 0;
						int TotalOpenNeighbors = 0;
						int OpenNeighbors[8] = { -1, -1, -1, -1, -1, -1, -1, -1 };

						for (int XOffset = -1; XOffset <= 1; XOffset++)
							for (int YOffset = -1; YOffset <= 1; YOffset++)
							{
								int Neighbor = (PressedY + YOffset)*Board.Width + (PressedX + XOffset);

								if ((PressedX + XOffset) < UI.XTiles && (PressedY + YOffset) < UI.YTiles &&
									(PressedX + XOffset) >= 0 && (PressedY + YOffset) >= 0)
								{
									if (Board.Tile[Neighbor].State == Board.Tile->FLAG)
									{
										FlaggedNeighbors++;
									}
									else if (Board.Tile[Neighbor].State == Board.Tile->NONE ||
											Board.Tile[Neighbor].State == Board.Tile->QUESTION)
									{
										OpenNeighbors[TotalOpenNeighbors] = Neighbor;
										TotalOpenNeighbors++;
									}

								}
							}

						if (TileContents == FlaggedNeighbors &&	TotalOpenNeighbors != 0)
						{
							for (int OpenNeighborIndex = 0; OpenNeighborIndex < TotalOpenNeighbors; OpenNeighborIndex++)
							{
								int Neighbor = OpenNeighbors[OpenNeighborIndex];

								//[NOTE] Even though the selected neighbors start in an unrevealed state
								//       It is possible that they are revealed by the floodfill of another
								//       Neighbor. Therefore, we need to check everytime to make sure we
								//       don't re-reveal them, and thus descreasing TilesToReveal too much.
								if (Board.Tile[Neighbor].State != Board.Tile->REVEALED)
								{
									Board.Tile[Neighbor].State = Board.Tile->REVEALED;
									TilesToReveal--;

									if (Board.Tile[Neighbor].Contents == MINE)
									{
										Board.Tile[Neighbor].Contents = MINE_RED;
										GameState = GS_LOSE;
									}
									else if (Board.Tile[Neighbor].Contents == NUM_0)
									{
										void* StackMemory = (game_board_data*)GameMemory->TransientStorage + (Board.Width*Board.Height);
										FloodFillEmptyNeighbors(Neighbor, StackMemory);
									}
								}
							}

						}

					}

				}
			}
			else if (GameInput->MouseControl == GameInput->MB_LEFT_DOWN)
			{
				if (Board.Tile[PressedTile].State == Board.Tile->NONE)
				{
					Board.Tile[PressedTile].State = Board.Tile->NONE_DOWN;
				}
				else if (Board.Tile[PressedTile].State == Board.Tile->QUESTION)
				{
					Board.Tile[PressedTile].State = Board.Tile->QUESTION_DOWN;
				}

				FaceState = FC_GUESS;
			}
			else if (GameInput->MouseControl == GameInput->MB_LEFT_UP)
			{
				if (Board.Tile[PressedTile].State == Board.Tile->NONE ||
					Board.Tile[PressedTile].State == Board.Tile->QUESTION)
				{
					Board.Tile[PressedTile].State = Board.Tile->REVEALED;
					TilesToReveal--;

 					if (Board.Tile[PressedTile].Contents == MINE)
					{
						Board.Tile[PressedTile].Contents = MINE_RED;
						GameState = GS_LOSE;
					}
					else if (Board.Tile[PressedTile].Contents == 0)
					{
						//[NOTE] Use part of the memory after the board to use as a stack
						void* StackMemory = (game_board_data*)GameMemory->TransientStorage + (Board.Width*Board.Height);
						FloodFillEmptyNeighbors(PressedTile, StackMemory);
					}
				}
			}
			else if (GameInput->MouseControl == GameInput->MB_RIGHT_DOWN)
			{
				if (Board.Tile[PressedTile].State == Board.Tile->NONE)
				{
					NotFlaggedMines--;
					Board.Tile[PressedTile].State = Board.Tile->FLAG;
				}
				else if (Board.Tile[PressedTile].State == Board.Tile->FLAG)
				{
					NotFlaggedMines++;
					Board.Tile[PressedTile].State = Board.Tile->QUESTION;
				}
				else if (Board.Tile[PressedTile].State == Board.Tile->QUESTION)
				{
					Board.Tile[PressedTile].State = Board.Tile->NONE;
				}
			}
			
		}
	}

}

void GameUpdateAndRender(game_memory* GameMemory, game_buffer* BackBuffer, game_raw_input* PreviousRawInput, game_raw_input* CurrentRawInput)
{
	//[TODO] Incomplete list probably
	//		 Last list update 2/3/2015
	//
	// scrollable game after a specific size? should be fun to make!
	//
	//High scores+Remember last game?(do i really want to write files in the user's pc for this little game?)
	//Maaaaaaaaaaybe more skins
	//Impossible to die in one move?
	//

	if (!GameMemory->IsInitialized)
	{
		GameOptions.BoardX = 9;
		GameOptions.BoardY = 9;
		GameOptions.Mines = 10;

		ExtractSpriteSheet(GameMemory->TransientStorage, GameMemory->PermanentStorage, SpriteMap);
		InitializeGame(BackBuffer, GameOptions.BoardX, GameOptions.BoardY, SpriteMap);
		GameMemory->IsInitialized = true;
		GameState = GS_NEW;
	}

	game_input GameInput = ProcessRawInput(PreviousRawInput, CurrentRawInput);
	HandleGameInput(&GameInput, BackBuffer, GameMemory);

	if (GameState == GS_NEW)
	{
		GameTime = {};
		FillGameBoard(GameMemory->TransientStorage, GameOptions.BoardX, GameOptions.BoardY, GameOptions.Mines);
		GameState = GS_RUNNING;
	}	

	if (GameState == GS_RUNNING)
	{	
		if (TilesToReveal == 0)
		{
			NotFlaggedMines = 0;
			GameState = GS_WIN;
		}

		if (GameTime.HasStarted)
		{
			GameTime.EndTime = clock();
			GameTime.SecondsElapsed = (uint32_t)((GameTime.EndTime - GameTime.StartTime) / CLOCKS_PER_SEC);
		}
	}

	DrawActiveSprites(BackBuffer);


}

