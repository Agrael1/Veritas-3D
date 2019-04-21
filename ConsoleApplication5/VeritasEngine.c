#include "Class.h"
#include "VeritasEngine.h"

bool _SetupScreen(void* self, Word width, Word height, Byte fontw, Byte fonth)
{
	struct c_class* this = self;

	// default setup for fast access
	if(this->Window->method->CreateConsole(this->Window, width, height, fontw, fonth)
		&& this->Window->method->SetCursor(this->Window, false))
		return true;
	else
		return false;
}

constructMethodTable(
	.SetupScreen = _SetupScreen
);

Constructor(void* self, va_list* ap)
{
	struct c_class* this = self;
	assignMethodTable(this);

	this->Window = new(ConsoleWindow);

	return this;
}

Destructor(void* self)
{
	struct c_class* this = self;
	delete(this->Window);
	return this;
}

ENDCLASSDESC