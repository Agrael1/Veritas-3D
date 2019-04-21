#ifndef OLCGAMEENGINE
#define OLCGAMEENGINE

#include "Frame.h"
#include "EngineCommons.h"

#define c_class olcGameEngine

enum PIXEL_TYPE
{
	PIXEL_SOLID = 0x2588,
	PIXEL_THREEQUARTERS = 0x2593,
	PIXEL_HALF = 0x2592,
	PIXEL_QUARTER = 0x2591,
};

struct sKeyState
{
	bool bPressed:1;
	bool bReleased:1;
	bool bHeld:1;
};

typedef struct
{
	// Virtual functions 

		// Creation handling
		bool(*OnUserCreate)(void* self);
		// Frame handling
		// *self - class pointer input
		// float fElapsedTime - time input for controls
		bool(*OnUserUpdate)(void* self, float fElapsedTime);
		// Optional for clean up *unused*
		bool(*OnUserDestroy)();

	// Internal class functions

		// Constructs console with input params:
		// @ _self - class pointer input
		// @ width, height - width and height of a console
		// @ fontw, fonth - width and height of the font on screen
		bool(*ConstructConsole)(void* _self, int width, int heigh, int fontw, int fonth);
		// Start routine
		// @ *_self - class pointer input
		void(*Start)(void* _self);
		// Prints character on the screen
		// @ _self - Game engine class pointer
		// @ x, y - coordinates of input charachter
		void(*PrintChar)(void* _self, int x, int y, wchar_t character, short color);

		int (*PrintStringW)(_Inout_updates_(m_buffscreen) void* _self, int x, int y, _In_reads_(length) const wchar_t *strptr, unsigned int length, short color);
		// Makes a part of a screen filled with specific character
		void(*Fill)(void* _self, int x1, int y1, int x2, int y2, wchar_t sym, short color);
		// Fills a center part of screen with symbol
		// Dimx, DimY - dimensions of center segment
		void(*FillCenter) (void* _self, int DimX, int DimY, wchar_t sym, short color);
		// Draw rectangle with 4 coordinates 
		// @ x1, y1 - from
		// @ x2, y2 - to
		// all the coordinates can be in random relation
		void (*DrawRectangle)(void* self, unsigned x1, unsigned y1, unsigned x2, unsigned y2, unsigned short color);
		void (*Compose)(void* self, struct Frame* localBuffer, int offsetX, int offsetY);

}vftb;

class 
{
	const void* _class;
	vftb* method;

	int m_nScreenWidth;
	int m_nScreenHeight;

	CHAR_INFO *m_bufScreen;

	HANDLE m_hOriginalConsole;
	CONSOLE_SCREEN_BUFFER_INFO m_OriginalConsoleInfo;
	HANDLE m_hConsole;
	HANDLE m_hConsoleIn;
	SMALL_RECT m_rectWindow;
	short m_keyOldState[256];
	short m_keyNewState[256];
	struct sKeyState *m_keys;

	struct sKeyState m_mouse[5];

	bool m_mouseOldState[5];
	bool m_mouseNewState[5];

	int m_mousePosX;
	int m_mousePosY;

	bool m_bConsoleInFocus:1;
	bool m_bEnableSound:1;
	bool m_bAtomActive:1;

};
#endif // !OLCGAMEENGINE


