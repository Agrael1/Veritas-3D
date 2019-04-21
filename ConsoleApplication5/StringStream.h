#pragma once
#include "EngineCommons.h"

#define c_class StringStream

class
{
	const void* _class;

	methods(
		struct c_class* (*Append)(void* self, const char* str);
		struct c_class* (*AppendI)(void* self, const long num);
		char* (*str)(const void* self);
		char* (*EndStr)(void* self);
	);

	privatev(
		char* inStr;
	    DWord len;
	);
};