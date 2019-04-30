#include <malloc.h>
#include <assert.h>
#include "Class.h"
#include "New.h"

void* new(const void* _class,...)
{
	const struct Class* rclass = _class;	// we need to convert pointer from void* to class* safely
	void *p = calloc(1, rclass->size);	// allocation of memory for class .using size param
	
	assert(p);							// if Null -> throw an error
	*(const struct Class**)p = rclass;	// safe assignment of class pointer to (value) of p, to have memory and built in funcs
	if (rclass->ctor)					// if has constructor with some dynal in it, execute with varargs on its input
	{
		va_list ap;
		va_start(ap, _class);			// 
		p = rclass->ctor(p, &ap);		// pass arguments as a list of pointers.
		va_end(ap);
	}
	return p;							//returns a pointer to class pointer (weird but worx)
}

void delete(void* self)
{
	const struct Class** cp = self;		// finds that little bastard (class pointer poiter)
	if (self&&*cp && (*cp)->dtor)		// if class has its own destructor the execute before freeing memory that class pointer occupies
		self = (*cp)->dtor(self);
	free(self);
}


unsigned int sizeOf(const void* _self)
{
	const struct Class* const * cp = _self;
	assert(_self&&*cp);
	return (*cp)->size;
}

const char * stringOf(const void * _self)
{
	const struct Class* const * cp = _self;
	assert(_self&&*cp);
	return (*cp)->typestring;
}

