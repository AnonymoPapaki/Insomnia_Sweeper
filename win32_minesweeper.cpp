#include<windows.h>
#include<windowsx.h>
#include"resource.h"

#include<stdint.h>
#include<stdio.h>
#include"helper.h"
#include "game.h"
#include "graphics.h"

#define Win32SpriteSheetResourceID MAKEINTRESOURCE(IDB_BITMAP1)
#define Win32IconResourceID MAKEINTRESOURCE(IDI_ICON1)

struct win32_buffer
{
	BITMAPINFO Info;
	void* Memory;
	int MemorySize;
	int Width;
	int Height;
};

struct win32_window_dimension
{
	int Width;
	int Height;
};

struct win32_window_buffer_rect
{
	int Width;
	int Height;
	int X;
	int Y;
};

static win32_buffer WindowBuffer;
static win32_window_buffer_rect WindowBufferRect;
static LONGLONG PerfCountFrequency;
static int Running;

static win32_window_dimension Win32GetWindowDimension(HWND Window)
{
	win32_window_dimension Result;

	RECT ClientRect;
	GetClientRect(Window, &ClientRect);
	Result.Width = ClientRect.right - ClientRect.left;
	Result.Height = ClientRect.bottom - ClientRect.top;

	return Result;
}

static void Win32GetBitmap24(HDC DeviceContext, HINSTANCE Instance, LPCTSTR ResourceID, void* Memory)
{
	HBITMAP BitmapHandle = (HBITMAP)LoadImage(Instance, ResourceID, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);

	if (BitmapHandle)
	{
		BITMAP Bitmap;
		GetObject(BitmapHandle, sizeof(BITMAP), &Bitmap);
			
		uint8_t* WritePos = (uint8_t*)Memory;
		uint8_t* ReadPos = (uint8_t*)Bitmap.bmBits;
		
		//[NOTE] First 8bytes are WidthxHeight of the bitmap.
		*WritePos = (uint8_t)Bitmap.bmWidth;
		WritePos += 4;
		*WritePos = (uint8_t)Bitmap.bmHeight;
		WritePos += 4;

		int PixelPos = 0;

		for (int Byte = 0; Byte < Bitmap.bmHeight*Bitmap.bmWidthBytes; Byte++)
		{
			*WritePos = *ReadPos;

			WritePos++;
			ReadPos++;

			if (PixelPos == 2)
			{
				//[NOTE] Adds the padding needed since sprites are 32bit and the
				//		 .bmp file is 24bit.
				WritePos++;
			}

			PixelPos = (PixelPos + 1) % 3;
		}

		DeleteObject(BitmapHandle);

	}
	else
	{
		OutputDebugString("[ERROR] Loading bitmap resource failed.");
	}
}

static void Win32InitializeWindowBuffer(win32_buffer* Buffer, int BufferMemorySize)
{
	if (Buffer->Memory)
	{
		VirtualFree(&Buffer->Memory, 0, MEM_RELEASE);
	}

	//[NOTE] These values will be changed by the game so we start
	//		 with some values that doen't matter.
	Buffer->Width = 1;
	Buffer->Height = 1;

	Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info);
	Buffer->Info.bmiHeader.biWidth = Buffer->Width;
	Buffer->Info.bmiHeader.biHeight = -Buffer->Height; //[NOTE] Negative height denotes top-down DIB
	Buffer->Info.bmiHeader.biPlanes = 1;
	Buffer->Info.bmiHeader.biBitCount = 32;
	Buffer->Info.bmiHeader.biCompression = BI_RGB;

	int BytesPerPixel = 4;
	Buffer->MemorySize = BufferMemorySize;
	Buffer->Memory = VirtualAlloc(0, Buffer->MemorySize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
}

inline void Win32ResizeWindowBuffer(win32_buffer* Buffer, int Width, int Height)
{
	Buffer->Width = Width;
	Buffer->Height = Height;
	Buffer->Info.bmiHeader.biWidth = Width;
	Buffer->Info.bmiHeader.biHeight = -Height; //[NOTE] Negative height denotes top-down DIB
}

static void Win32DisplayBufferInWindow(HDC DeviceContext, win32_buffer* Buffer, int Width, int Height)
{
	float BufferAspectRatio = (float)Buffer->Width / (float)Buffer->Height;
	float WindowAspectRatio = (float)Width / Height;

	WindowBufferRect.Width = Width;
	WindowBufferRect.Height = Height;
	WindowBufferRect.X = 0;
	WindowBufferRect.Y = 0;

	if (WindowAspectRatio > BufferAspectRatio)
	{
		WindowBufferRect.Width = (int)(BufferAspectRatio*Height);
		WindowBufferRect.X = (Width - WindowBufferRect.Width) / 2;

		PatBlt(DeviceContext, 0, 0, WindowBufferRect.X, Height, BLACKNESS);
		PatBlt(DeviceContext, WindowBufferRect.X + WindowBufferRect.Width, 0, Width - (WindowBufferRect.Width + WindowBufferRect.X), Height, BLACKNESS);
	}
	else
	{
		WindowBufferRect.Height = (int)(Width / BufferAspectRatio);
		WindowBufferRect.Y = (Height - WindowBufferRect.Height) / 2;

		PatBlt(DeviceContext, 0, 0, Width, WindowBufferRect.Y, BLACKNESS);
		PatBlt(DeviceContext, 0, WindowBufferRect.Y + WindowBufferRect.Height, Width, Height - (WindowBufferRect.Height + WindowBufferRect.Y), BLACKNESS);
	}

	StretchDIBits(
		DeviceContext,
		WindowBufferRect.X, WindowBufferRect.Y, WindowBufferRect.Width, WindowBufferRect.Height,
		0, 0, Buffer->Width, Buffer->Height,
		Buffer->Memory, &Buffer->Info,
		DIB_RGB_COLORS, SRCCOPY);
}

static LRESULT CALLBACK Win32MainWindowCallback(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
	LRESULT Result = 0;

	switch (Message)
	{
		case WM_CLOSE:
		{
			PostQuitMessage(0);
		}

		case WM_PAINT:
		{
			PAINTSTRUCT Paint;
			HDC DeviceContext = BeginPaint(Window, &Paint);
			win32_window_dimension WindowDimension = Win32GetWindowDimension(Window);
			Win32DisplayBufferInWindow(DeviceContext, &WindowBuffer, WindowDimension.Width, WindowDimension.Height);
			EndPaint(Window, &Paint);
		}

		default:
		{
			Result = DefWindowProc(Window, Message, WParam, LParam);
		} break;

	}

	return Result;
}

static void Win32ProcessPendingMessages(HWND Window, game_raw_input* GameRawInput)
{
	MSG Message;

	while (PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
	{
		switch (Message.message)
		{
			case WM_QUIT:
			{
				Running = false;
			} break;

			case WM_KEYUP:
			{
				WPARAM KeyCode = Message.wParam;

				if (KeyCode == VK_UP)
				{
					GameRawInput->KeyUp = GameRawInput->KEYB_UP;
				}
				else if (KeyCode == VK_DOWN)
				{
					GameRawInput->KeyUp = GameRawInput->KEYB_DOWN;
				}
				else if (KeyCode == VK_LEFT)
				{
					GameRawInput->KeyUp = GameRawInput->KEYB_LEFT;
				}
				else if (KeyCode == VK_RIGHT)
				{
					GameRawInput->KeyUp = GameRawInput->KEYB_RIGHT;
				}
				else if (KeyCode == VK_ADD)
				{
					GameRawInput->KeyUp = GameRawInput->KEYB_PLUS;
				}
				else if (KeyCode == VK_SUBTRACT)
				{
					GameRawInput->KeyUp = GameRawInput->KEYB_MINUS;
				}
				else if (KeyCode == VK_MULTIPLY)
				{
					GameRawInput->KeyUp = GameRawInput->KEYB_SUGGEST;
				}
				else
				{
					GameRawInput->KeyUp = GameRawInput->KEYB_OTHER;
				}

			} break;

			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP:
			case WM_RBUTTONDOWN:
			case WM_RBUTTONUP:
			{
				int MouseX = GET_X_LPARAM(Message.lParam);
				int MouseY = GET_Y_LPARAM(Message.lParam);

				GameRawInput->MouseX = (int)(((float)(MouseX - WindowBufferRect.X) / WindowBufferRect.Width) * WindowBuffer.Width);
				GameRawInput->MouseY = (int)(((float)(MouseY - WindowBufferRect.Y) / WindowBufferRect.Height) * WindowBuffer.Height);						 
						 
				GameRawInput->LeftMouseUp = (Message.message == WM_LBUTTONUP);
				GameRawInput->LeftMouseDown = (Message.wParam & MK_LBUTTON) != 0;
						 
				GameRawInput->RightMouseUp = (Message.message == WM_RBUTTONUP);
				GameRawInput->RightMouseDown = (Message.wParam & MK_RBUTTON) != 0;

				if (Message.message == WM_LBUTTONDOWN)
				{
					SetCapture(Window);
				}

				if (Message.message == WM_LBUTTONUP)
				{
					GameRawInput->RightMouseDown = false;
					ReleaseCapture();
				}

				if (Message.message == WM_RBUTTONUP)
				{
					GameRawInput->LeftMouseDown = false;
				}

				if (Message.wParam & MK_LBUTTON)
				{
					GameRawInput->RightMouseUp = false;
				}

				if (Message.wParam & MK_RBUTTON)
				{
					GameRawInput->LeftMouseUp = false;
				}


			} break;

			case WM_MOUSEMOVE:
			{
				if (Message.wParam & MK_LBUTTON)
				{
					//[TODO] This is the same code as above
					int MouseX = GET_X_LPARAM(Message.lParam);
					int MouseY = GET_Y_LPARAM(Message.lParam);

					GameRawInput->MouseX = (int)(((float)(MouseX - WindowBufferRect.X) / WindowBufferRect.Width) * WindowBuffer.Width);
					GameRawInput->MouseY = (int)(((float)(MouseY - WindowBufferRect.Y) / WindowBufferRect.Height) * WindowBuffer.Height);
				}

			} break;

			default:
			{
				TranslateMessage(&Message);
				DispatchMessage(&Message);
			} break;
		}
	}

}

inline LARGE_INTEGER Win32GetWallClock()
{
	LARGE_INTEGER Result;
	QueryPerformanceCounter(&Result);
	return Result;
}

inline float Win32GetSecondsElapsed(LARGE_INTEGER Start, LARGE_INTEGER End)
{
	return (float)(End.QuadPart - Start.QuadPart) / (float)PerfCountFrequency;
}

int CALLBACK WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, LPSTR CommandLine, int CommandShow)
{
	LARGE_INTEGER PerfCounterFrequencyResult;
	QueryPerformanceFrequency(&PerfCounterFrequencyResult);
	PerfCountFrequency = PerfCounterFrequencyResult.QuadPart;

	WNDCLASS WindowClass = {};

	WindowBuffer = {};

	//[TODO] Perhaps allow for size to be determined based on some
	//		 predefined maximum allowed game sizes, instead of having it
	//		 hardcoded. 12MB is just enough for a maximum 100x100 game.
	Win32InitializeWindowBuffer(&WindowBuffer, Megabytes(12)); 

	WindowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	WindowClass.lpfnWndProc = Win32MainWindowCallback;
	WindowClass.hInstance = Instance;
	WindowClass.lpszClassName = "MinesweeperWindowClass";
	WindowClass.hIcon = LoadIcon(Instance, Win32IconResourceID); 
	WindowClass.hCursor = LoadCursor(0, IDC_ARROW);

	if (RegisterClass(&WindowClass))
	{
		HWND Window = CreateWindowEx(
						0,
						WindowClass.lpszClassName,
						"Minesweeper",
						WS_OVERLAPPEDWINDOW | WS_VISIBLE,
						CW_USEDEFAULT, CW_USEDEFAULT,
						600,600,
						0, 0, Instance,	0
						);

		if (Window)
		{
			HDC DeviceContext = GetDC(Window);

			game_memory GameMemory = {};
			//[TODO] These aren't tightened and maybe be well bigger than needed.
			GameMemory.PermanentStorageSize = Kilobytes(64);
			GameMemory.TransientStorageSize = Kilobytes(512);
			int TotalGameMemorySize = GameMemory.PermanentStorageSize + GameMemory.TransientStorageSize;

			GameMemory.PermanentStorage = VirtualAlloc(0, TotalGameMemorySize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
			GameMemory.TransientStorage = (uint8_t*)GameMemory.PermanentStorage + GameMemory.PermanentStorageSize;

			Win32GetBitmap24(DeviceContext, Instance, Win32SpriteSheetResourceID, GameMemory.TransientStorage);

			if (GameMemory.PermanentStorage && GameMemory.TransientStorage)
			{
				Running = true;

				LARGE_INTEGER LastCounter = Win32GetWallClock();

				//[TODO] Do I care about matching screen Hz for this game?
				//		 As it is, it will try to cap frames/s to 60.
				int GameUpdateHz = 60;
				float TargetSecondsPerFrame = 1.0f / GameUpdateHz;
				
				game_raw_input CurrGameInput = {};
				game_raw_input PrevGameInput = {};

				while (Running)
				{
					Win32ProcessPendingMessages(Window, &CurrGameInput);

					game_buffer GameBackBuffer;
					GameBackBuffer.Memory = WindowBuffer.Memory;
					GameBackBuffer.MemorySize = WindowBuffer.MemorySize;
					GameBackBuffer.Width = WindowBuffer.Width;
					GameBackBuffer.Height = WindowBuffer.Height;

					GameUpdateAndRender(&GameMemory, &GameBackBuffer, &PrevGameInput, &CurrGameInput);

					//[NOTE] Use buffer dimenstions specified by the game.
					Win32ResizeWindowBuffer(&WindowBuffer, GameBackBuffer.Width, GameBackBuffer.Height);


					float SecondsElapsedForFrame = Win32GetSecondsElapsed(LastCounter, Win32GetWallClock());

					if (SecondsElapsedForFrame < TargetSecondsPerFrame)
					{
						//[TODO] Do we really need to set beginTimePeriod to ensure better
						//		 resolution of Sleep? Since it's a system wide change(I think!)
						DWORD SleepTime = (DWORD)(1000.0f*(TargetSecondsPerFrame - SecondsElapsedForFrame));
						Sleep(SleepTime);
					}
					else
					{
						OutputDebugString("[Warning] Frame took too long to render.");
					}

					win32_window_dimension WindowDimension = Win32GetWindowDimension(Window);
					Win32DisplayBufferInWindow(DeviceContext, &WindowBuffer, WindowDimension.Width, WindowDimension.Height);

					PrevGameInput = CurrGameInput;

					//[NOTE] If we don't reset the following, the messages will keep being send
					//		 after being fired once. For some messages this may be something we
					//		 want to do however(like holding down a mouse button)
					CurrGameInput.KeyUp = CurrGameInput.KEYB_OTHER;
					CurrGameInput.LeftMouseUp = false;
					CurrGameInput.RightMouseUp = false;
					if(!CurrGameInput.LeftMouseDown) 
						CurrGameInput.RightMouseDown = false; //[NOTE] Stop sending this message only after RL input

					LARGE_INTEGER EndCounter = Win32GetWallClock();

#if 0
					float FramesPerSecond = 1.0f / Win32GetSecondsElapsed(LastCounter, EndCounter);
					char FPSBuffer[64];
					sprintf_s(FPSBuffer, "%.2ffps\n", FramesPerSecond);
					OutputDebugString(FPSBuffer);
#endif
					LastCounter = EndCounter;
				}
			}
			else
			{
				OutputDebugString("[ERROR] Game memory allocation failed.");
			}
		}
		else
		{
			OutputDebugString("[ERROR] Window creation failed.");
		}
	}
	else
	{
		OutputDebugString("[ERROR] Class registration failed.");
	}

	return 0;
}