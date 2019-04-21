#pragma once
#include "WinSetup.h"
#include "Exception.h"
#include "EngineCommons.h"

#define c_class ConsoleWindow

extern const void* WindowException;

class 
{
	const void* _class;

	// Exceptions //
#pragma region Exceptions
#undef c_class
#define c_class WindowException

	struct c_class
	{
		struct Exception _;

		privatev(
			HRESULT hr;
		);
	};
	// End Exception //
#undef c_class
#define c_class ConsoleWindow
#pragma endregion

	HANDLE hOut;	// for output
	HANDLE hIn;		// for mouse

	HANDLE hOriginalConsole;
	SMALL_RECT rWindowRect;

	HWND consoleWindow;

	Word Width;
	Word Height;

	bool bMouse;
	bool bCursor;

	methods(
		bool(*CreateConsole)(void* self, Word width, Word height, const Byte fontw, const Byte fonth);
		bool(*SetCursor)(void* self, bool value);
		bool(*Restore)(const void* self);
	);
};

#define WND_EXCEPT_AUTO() new(WindowException,__LINE__, __FILE__, GetLastError())