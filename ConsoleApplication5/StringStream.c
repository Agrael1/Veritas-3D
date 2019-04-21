#include "Class.h"
#include "StringStream.h"
#include <string.h>
#include <malloc.h>
#include <math.h>

inline Word Strlen(const char* str)
{
	return str != NULL ? (Word)strlen(str) : 0;
}

inline int get_len(const int n)
{
	register a = n;
	if (n == 0)
		return 1;

	if (n < 0)
	{
		a = -a; // if negative
	}

	return  (int)log10(n) + 1;
}

inline void itoa_s(char* const buffer, const long n, const Word len)
{
	register a = n;
	buffer[len - 1] = '\0';
	for (int i = len-2; i >= 0; i--)
	{
		buffer[i] = a % 10 + 48;
		a /= 10;
	}
}

struct c_class* _Append_s(void* self, const char* str, const DWord length)
{
	struct c_class* this = self;

	private.inStr = realloc(private.inStr, length + private.len+1);
#pragma warning(disable:4996)
	strcpy(private.inStr + private.len, str);
#pragma warning(default:4996)
	private.len += length;

	return this;
}

struct c_class* _Append(void* self, const char* str)
{
	struct c_class* this = self;
	_Append_s(this, str, Strlen(str));
	return this;
}

struct c_class* _AppendI(void* self, const long num)
{
	struct c_class* this = self;
	// loading buffer length of a number
	register Word local = get_len(num)+1;
	char* str = malloc(local);
	itoa_s(str,num,local);

	_Append_s(this, str, local-1);

	free(str);

	return this;
}

char* _str(const void* self)
{
	const struct c_class* this = self;
	return private.inStr;
}

// Warning this one is self destruct last breath string yield function,
// it is callable when you need string as a return value
// but don't need a string stream to continue existing
char* _EndStr(void* self)
{
	struct c_class* this = self;
	char* _proxy = private.inStr;
	private.inStr = NULL;
	delete(this);
	return _proxy;
}

constructMethodTable
(
	.Append = _Append,
	.AppendI = _AppendI,
	.str = _str,
	.EndStr = _EndStr
);

Constructor(void* self, va_list* ap)
{
	struct c_class* this = self;
	assignMethodTable(this);

	private.inStr = NULL;
	private.len = 0;
	return this;
}
Destructor(void* self)
{
	struct c_class* this = self;
	if (private.inStr != NULL)
		free(private.inStr);
	return self;
}
ENDCLASSDESC