#pragma once

#include "WinSetup.h"
#include "Window.h"
#include "EngineCommons.h"

#define c_class VeritasEngine

class
{
	const void* _class;
	struct ConsoleWindow* Window;



	methods(
		bool(*SetupScreen)(void* self, Word width, Word height, Byte fontw, Byte fonth);
	);
};