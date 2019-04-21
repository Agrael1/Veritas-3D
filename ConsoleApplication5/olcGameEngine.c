#include "Exception.h"
#include "olcGameEngine.h"
#include <time.h>
#include <stdio.h>
#include "Class.h"

// Internal functions. Paste functions here:
// Clipping wrap
void Clip(void* self, int *x, int *y)
{
	struct c_class* this = self;
	if (*x < 0) *x = 0;
	if (*x >= this->m_nScreenWidth) *x = this->m_nScreenWidth;
	if (*y < 0) *y = 0;
	if (*y >= this->m_nScreenHeight) *y = this->m_nScreenHeight;
}

#pragma region Internal Functions
// Fill function
void Fill(void* _self, int x1, int y1, int x2, int y2, wchar_t sym, short color)
{
	struct c_class* this = _self;
	Clip(this, &x1, &y1);
	Clip(this, &x2, &y2);
	{
		for (int x = x1; x < x2; x++)
			for (int y = y1; y < y2; y++)
			{
				this->m_bufScreen[y*this->m_nScreenWidth + x].Char.UnicodeChar = sym;
				this->m_bufScreen[y*this->m_nScreenWidth + x].Attributes = color;
			}
	}
}
// Game thread function
DWORD _stdcall GameThread(void* _self)
{
	struct c_class* this = _self;

	clock_t tp1, tp2;
	/* Recording the starting clock tick.*/
	tp1 = clock();
	tp2 = clock();

	// Create user resources as part of this thread
	if (!this->method->OnUserCreate(this))
		this->m_bAtomActive = false;

	while (this->m_bAtomActive)
	{
		// Time handle
		tp2 = clock();
		float fElapsedTime = (float)(tp2 - tp1) / (float)(CLOCKS_PER_SEC);
		tp1 = tp2;
		

		for (int i = 0; i < 256; i++)
		{
			this->m_keyNewState[i] = GetAsyncKeyState(i);

			this->m_keys[i].bPressed = false;
			this->m_keys[i].bReleased = false;

			if (this->m_keyNewState[i] != this->m_keyOldState[i])
			{
				if (this->m_keyNewState[i] & 0x8000)
				{
					this->m_keys[i].bPressed = !this->m_keys[i].bHeld;
					this->m_keys[i].bHeld = true;
				}
				else
				{
					this->m_keys[i].bPressed = true;
					this->m_keys[i].bHeld = false;
				}
			}
			this->m_keyOldState[i] = this->m_keyNewState[i];
		}

		INPUT_RECORD inBuf[32];
		DWORD events = 0;
		GetNumberOfConsoleInputEvents(this->m_hConsoleIn, &events);

		if (events > 0)
			ReadConsoleInput(this->m_hConsoleIn, inBuf, events, &events);

		for (DWORD i = 0; i < events; i++)
		{
			switch (inBuf[i].EventType)
			{
			case FOCUS_EVENT:
			{
				this->m_bConsoleInFocus = inBuf[i].Event.FocusEvent.bSetFocus;
			}
			break;

			case MOUSE_EVENT:
			{
				switch (inBuf[i].Event.MouseEvent.dwEventFlags)
				{
				case MOUSE_MOVED:
				{
					this->m_mousePosX = inBuf[i].Event.MouseEvent.dwMousePosition.X;
					this->m_mousePosY = inBuf[i].Event.MouseEvent.dwMousePosition.Y;
				}
				break;

				case 0:
				{
					for (int m = 0; m < 5; m++)
						this->m_mouseNewState[m] = (inBuf[i].Event.MouseEvent.dwButtonState & (1 << m)) > 0;

				}
				break;

				default:
					break;
				}
			}
			break;

			default:
				break;
				// We don't care just at the moment
			}
		}

		// Handle frame update
		if (!this->method->OnUserUpdate(this,fElapsedTime))
			this->m_bAtomActive = false;
		WCHAR s[40];
		swprintf_s(s,40,L"Custom OLC Engine test-%3.2f", 1.0 / fElapsedTime);
		SetConsoleTitle(s);
		WriteConsoleOutputW(this->m_hConsole, this->m_bufScreen, (COORD){ (short)this->m_nScreenWidth, (short)this->m_nScreenHeight }, (COORD){ 0,0 }, &this->m_rectWindow);
	}
	return 0;
}
// Constructs console with input params
bool ConstructConsole(void* _self, int width, int heigh, int fontw, int fonth)
{
	struct c_class* this = _self;
	this->m_nScreenHeight = heigh;
	this->m_nScreenWidth = width;
	bool bMaxWindow = false;

	CONSOLE_CURSOR_INFO info;
	info.dwSize = 100;
	info.bVisible = FALSE;
	SetConsoleCursorInfo(this->m_hConsole, &info);

	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;
	cfi.dwFontSize.X = fontw;
	cfi.dwFontSize.Y = fonth;
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_NORMAL;
	wcscpy_s(cfi.FaceName, 9, L"Consolas");

	if (!SetCurrentConsoleFontEx(this->m_hConsole, bMaxWindow, &cfi))
	{
		throw( new(Exception, __LINE__, __FILE__)); 
	}

	COORD coordLargest = GetLargestConsoleWindowSize(this->m_hConsole);
	if (this->m_nScreenHeight > coordLargest.Y)
		return -2;
	if (this->m_nScreenWidth > coordLargest.X)
		return -3;

	COORD buffer = { (short)this->m_nScreenWidth,(short)this->m_nScreenHeight };
	if (!SetConsoleScreenBufferSize(this->m_hConsole, buffer))
		return -4;
	this->m_rectWindow = (SMALL_RECT){ 0,0,(short)this->m_nScreenWidth - 1,(short)this->m_nScreenHeight - 1 };
	if (!SetConsoleWindowInfo(this->m_hConsole, TRUE, &this->m_rectWindow))
		return -5;

	this->m_bufScreen = malloc(this->m_nScreenHeight*this->m_nScreenWidth * sizeof(CHAR_INFO));

	// init buffer
	if ((*this).m_bufScreen)
		Fill(this, 0, 0, this->m_nScreenWidth, this->m_nScreenHeight, L' ', BG_BLACK);

	if (!SetConsoleMode(this->m_hConsoleIn, ENABLE_EXTENDED_FLAGS | ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT))
		return -6;

	// set window as not resizeable
	HWND consoleWindow = GetConsoleWindow();
	SetWindowLong(consoleWindow, GWL_STYLE, GetWindowLong(consoleWindow, GWL_STYLE) & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX );

	// For mouse to be captured after leaving the screen
	SetCapture(consoleWindow);
	
	return 1;
}
// Start rouitine, assembles a thread and gives it a handle
void Start(void* _self)
{
	DWORD dwThreadID;
	HANDLE hThread;

	struct c_class* this = _self;
	this->m_bAtomActive = true;

	hThread = CreateThread(
		NULL,
		0,
		&GameThread,
		_self,
		0,
		&dwThreadID);

	WaitForSingleObject(hThread, INFINITE);
}
#pragma endregion

#pragma region Print Functions
// Symbolic print function
void PrintChar(void* _self, int x, int y, wchar_t character, short color)
{
	struct c_class* this = _self;
	if (x >= 0 && x < this->m_nScreenWidth&&y >= 0 && y < this->m_nScreenHeight)
	{
		this->m_bufScreen[y*this->m_nScreenWidth + x].Char.UnicodeChar = character;
		this->m_bufScreen[y*this->m_nScreenWidth + x].Attributes = color;
	}
}

// Prints string of wide char
_Success_(return == length)
int PrintStringW( _Inout_updates_(m_buffscreen) void* _self, int x, int y, _In_reads_(length) const wchar_t *strptr, unsigned int length, short color)
{
	struct c_class* this = _self;
	Clip(this, &x, &y);

	for (unsigned int i = 0; i < length; i++)
	{
		if ((i+x) != this->m_nScreenWidth - 1)
		{
			this->m_bufScreen[y*this->m_nScreenWidth + x].Char.UnicodeChar = strptr[i];
			this->m_bufScreen[y*this->m_nScreenWidth + x].Attributes = color; x++;
		}
		else 
			if(y+1<=this->m_nScreenHeight)
				{ y++; x = 0; i--; continue; }
			else return i;		
	}
	return length;
}

void FillCenter(void* _self, int DimX, int DimY, wchar_t sym, short color)
{
	struct c_class* this = _self;
	Clip(this, &DimX, &DimY);

	for(int y = (int)((this->m_nScreenHeight-DimY)/2);y<(this->m_nScreenHeight - DimY) / 2+DimY; y++)
		for (int x = (int)((this->m_nScreenWidth - DimX) / 2); x < (this->m_nScreenWidth - DimX) / 2 + DimX; x++)
		{
			this->m_bufScreen[y*this->m_nScreenWidth + x].Char.UnicodeChar = sym;
			this->m_bufScreen[y*this->m_nScreenWidth + x].Attributes = color;
		}
}
#pragma endregion

#pragma region Draw Functions
void DrawRectangle(void* self, unsigned x1, unsigned y1, unsigned x2, unsigned y2, unsigned short color)
{
	struct c_class *this = self;
	if ((x2 <= x1) && (y2 >= y1))
		this->method->Fill(this, x2, y1, x1, y2, L' ', color);
	else if ((x2 >= x1) && (y2 <= y1))
		this->method->Fill(this, x1, y2, x2, y1, L' ', color);
	else if ((x2 <= x1)&&(y2 <= y1))
		this->method->Fill(this, x2, y2, x1, y1, L' ', color);
	else
		this->method->Fill(this, x1, y1, x2, y2, L' ', color);
}

void Compose(void* self, struct Frame* localBuffer, int offsetX, int offsetY)
{
	struct c_class *this = self;
	if(offsetX<this->m_nScreenWidth&&offsetY<this->m_nScreenWidth)
		for(int j = offsetY; j<localBuffer->nFrameHeight; j++)
			for (int i = offsetX; i<localBuffer->nFrameLength; i++)
			{

				this->m_bufScreen[j*this->m_nScreenWidth + i] = localBuffer->localFrame[(j-offsetY)*(localBuffer->nFrameLength) + i-offsetX];
			}
}

#pragma endregion

// Function table with early binding
vftb _method = {
	.ConstructConsole = ConstructConsole,
	.Start = Start,
	.PrintChar = PrintChar,
	.Fill = Fill,
	.FillCenter = FillCenter,
	.PrintStringW = PrintStringW,
	.DrawRectangle = DrawRectangle,
	.Compose = Compose
};

// Constructor (must be last to bind methods)
void* olcGameEngine_ctor(void* _self, va_list *app)
{
	struct olcGameEngine* this = _self;

	this->m_nScreenWidth = 80;
	this->m_nScreenHeight = 30;

	this->m_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	this->m_hConsoleIn = GetStdHandle(STD_INPUT_HANDLE);

	this->m_keys = (struct sKeyState*)calloc(256, sizeof(struct sKeyState));
	this->method = &_method;
	this->m_bEnableSound = false;
	this->m_bConsoleInFocus = true;
	return this;
}
// Destructor
void* olcGameEngine_dtor(void* self)
{
	struct olcGameEngine *this = self;
	free(this->m_keys);

	SetConsoleActiveScreenBuffer(this->m_hOriginalConsole);
	free(this->m_bufScreen);
	return this;
}

static const struct Class _olcGameEngine =
{
	sizeof(struct c_class), 
	olcGameEngine_ctor, olcGameEngine_dtor
};
const void* olcGameEngine = &_olcGameEngine;