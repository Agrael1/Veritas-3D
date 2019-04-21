#include "Class.h"
#include "StringStream.h"
#include "Exception.h"
#include <malloc.h>

setupException

const char* virtual(GetType)() 
{
	return "Veritas Exception\n\r";
}

char* _GetOriginString(const void* self)
{
	const struct c_class* this = self;
	struct StringStream *oss = new(StringStream);
	oss->method->Append(oss, "[File]: ")->method->Append(oss,this->file)
		->method->Append(oss,"\n\r[Line]: ")->method->AppendI(oss,this->line);
	return oss->method->EndStr(oss);
}

char* virtual(what) (void* self)
{
	struct c_class* this = self;
	struct StringStream *oss = new(StringStream);
	char* _proxy = _GetOriginString(this);
	oss->method->Append(oss,this->method->GetType())->method->Append(oss, _proxy);

	this->whatBuffer = oss->method->EndStr(oss);

	free(_proxy);
	return this->whatBuffer;
}

constructMethodTable(
	.GetType = virtual(GetType),
	.what = virtual(what),
	.GetOriginString = _GetOriginString
);

Constructor(void* self, va_list* ap)
{
	struct c_class* this = self;
	assignMethodTable(this);
	this->line = __crt_va_arg(*ap, Word);
	this->file = __crt_va_arg(*ap, const char*);
	this->whatBuffer = NULL;
	return this;
}
Destructor(void* self)
{
	struct c_class* this = self;
	if (this->whatBuffer)
		free(this->whatBuffer);
	return this;
}

ENDCLASSDESC