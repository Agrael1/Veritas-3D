#pragma once
#include "EngineCommons.h"

#define c_class BitField

class 
{
	const void* _class;
	MaxInt* BitArray;
	DWord Positions;

	methods(
		bool(*Any)(void* self);
		bool(*IsSet)(void* self, DWord pos);
		bool(*Set)(void* self, DWord pos);
		bool(*Reset)(void* self, DWord pos);
		bool(*FullReset)(void* self);
		);
};

#if _WIN64
#define BLOCKSZ 64
#else
#define BLOCKSZ 32
#endif