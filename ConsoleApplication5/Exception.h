#pragma once
#include <setjmp.h>
#include "EngineCommons.h"

#define c_class Exception

class
{
	const void* _class;
	int line;
	const char* file;
	
	methods(
		char* (*GetType)();
		char* (*what)(void* self);
		char* (*GetOriginString)(const void* self);
	);

	char* whatBuffer;
};

// macros for supporting exceptions
#define setupException jmp_buf env = { 0 }; struct Exception *__e = NULL;
#define try if (!setjmp(env))  
#define catch(x,y) else { struct x* y = __e; if(__e) 
#define endtry } if(__e) {delete(__e); __e = NULL;}
#define throw(x) {__e = x; longjmp(env,1);}

extern  jmp_buf env;
extern  struct Exception *__e;