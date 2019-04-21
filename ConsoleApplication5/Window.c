#include "Class.h"
#include "StringStream.h"
#include "Window.h"
#include <stdio.h>

// namespace ConsoleWindow
#define c_class ConsoleWindow

bool _Restore(const void* self)
{
	const struct c_class *this = self;
	if (!SetConsoleActiveScreenBuffer(this->hOriginalConsole))
		return false;
	return true;
}
bool _SetFont(const void* self, const Byte fontw, const Byte fonth)
{
	const struct c_class *this = self;

	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;
	cfi.dwFontSize.X = fontw;
	cfi.dwFontSize.Y = fonth;
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_NORMAL;

	wcscpy_s(cfi.FaceName, 9, L"Consolas");

	if (!SetCurrentConsoleFontEx(this->hOut, false, &cfi))
		return 0;

	return 1;
}
bool _SetCursor(void* self, bool value)
{
	struct c_class *this = self;
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 100;
	info.bVisible = value;
	if (!SetConsoleCursorInfo(this->hOut, &info))
		return false;
	return true;
}
bool _CreateConsole(void* self, Word width, Word height, const Byte fontw, const Byte fonth)
{
	struct c_class *this = self;

	this->Height = height;
	this->Width = width;

	this->hOut = CreateConsoleScreenBuffer(
		GENERIC_READ |           // read/write access 
		GENERIC_WRITE,
		FILE_SHARE_READ |
		FILE_SHARE_WRITE,        // shared 
		NULL,                    // default security attributes 
		CONSOLE_TEXTMODE_BUFFER, // must be TEXTMODE 
		NULL);

	if (this->hOut == INVALID_HANDLE_VALUE)
	{
		throw(WND_EXCEPT_AUTO());
	}

	if (!SetConsoleActiveScreenBuffer(this->hOut))
	{
		throw(WND_EXCEPT_AUTO());
	}

	// Set flags to allow mouse input		
	if (!SetConsoleMode(this->hIn, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT))
	{
		throw(WND_EXCEPT_AUTO());
	}
		

	if(!_SetFont(this, fontw, fonth))
	{
		throw(WND_EXCEPT_AUTO());
	}

	COORD coordLargest = GetLargestConsoleWindowSize(this->hOut);
	if (height > coordLargest.Y)
		this->Height = height = coordLargest.Y;
	if (width > coordLargest.X)
		this->Height = width = coordLargest.X;

	// Set size of buffer
	SetConsoleScreenBufferSize(this->hOut, (COORD) { width, height });

	// Set Physical Console Window Size
	this->rWindowRect = (SMALL_RECT){ 0, 0, (short)width - 1, (short)height - 1 };
	if (!SetConsoleWindowInfo(this->hOut, TRUE, &this->rWindowRect))
	{
		throw(WND_EXCEPT_AUTO());
	}

	return true;
}

constructMethodTable(
	.CreateConsole = _CreateConsole,
	.SetCursor = _SetCursor,
	.Restore = _Restore
);

Constructor(void* self, va_list* ap)
{
	struct c_class *this = self;
	assignMethodTable(this);

	this->hIn = GetStdHandle(STD_INPUT_HANDLE);
	this->hOriginalConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	this->hOut = this->hOriginalConsole;
	this->bMouse = false;
	this->bCursor = true;

	return this;
}
Destructor(void* self)
{
	_Restore(self);
	return self;
}
ENDCLASSDESC

// namespace WindowException
#undef c_class
#define c_class WindowException
#define base (this->_)

const char* virtual(GetType)()
{
	return "Console Window Exception\n\r";
}
char* TranslateErrorCode(HRESULT hr)
{
	char* pMsgBuf = NULL;
	DWORD nMsgLen = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS, NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), &pMsgBuf,
		0, NULL);

	if (nMsgLen == 0)
	{
		return "Unknown error";
	}
	return pMsgBuf;
}
char* GetErrorString(void* self)
{
	struct c_class* this = self;
	return TranslateErrorCode(private.hr);
}
char* virtual(what)(void* self)
{
	struct c_class* this = self;
	struct StringStream *oss = new(StringStream);
	char* _proxy = GetErrorString(this);
	char* _origin = base.method->GetOriginString(this);

	oss->method->Append(oss, base.method->GetType())
		->method->Append(oss, "[Error Code]: ")->method->AppendI(oss, private.hr)
		->method->Append(oss, "\n[Description]: ")->method->Append(oss, _proxy)->method->Append(oss,"\n")
		->method->Append(oss, _origin);

	LocalFree(_proxy);
	free(_origin);

	base.whatBuffer = oss->method->EndStr(oss);
	return base.whatBuffer;
}
Constructor(void* self, va_list* ap)
{
	struct c_class *this = ((struct Class*)Exception)->ctor(self,ap);
	private.hr = va_arg(*ap, HRESULT);

	base.method->what = virtual(what);
	base.method->GetType = virtual(GetType);

	return this;
}
Destructor(void* self)
{
	struct c_class *this = ((struct Class*)Exception)->dtor(self);
	return this;
}
ENDCLASSDESC 