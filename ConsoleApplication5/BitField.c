#include "BitField.h"
#include <stdarg.h>
#include <malloc.h>
#include "Class.h"

inline Byte CalcrPos(DWord pos) 
{
	return pos / BLOCKSZ;
}
inline DWord CalcdPos(DWord pos)
{
	return 1i64 << (pos%BLOCKSZ);
}

bool _IsSet(void* self, DWord pos)
{
	struct c_class *this = self;
	if (pos <= this->Positions)
		return this->BitArray[CalcrPos(pos)]&CalcdPos(pos);
	else
		return false;
}
bool _Any(void* self)
{
	struct c_class *this = self;
	for (DWord i = 0; i < this->Positions; i++)
	{
		if (_IsSet(this, i))
		{
			return true;
		}
	}
	return false;
}
bool _Set(void* self, DWord pos)
{
	struct c_class *this = self;
	if (pos <= this->Positions)
	{
		this->BitArray[CalcrPos(pos)]|=CalcdPos(pos);
		return true;
	}
	else
		return false;
}
bool _Reset(void* self, DWord pos)
{
	struct c_class *this = self;
	if (pos <= this->Positions)
	{
		this->BitArray[CalcrPos(pos)] &= CalcdPos(pos);
		return true;
	}
	else
		return false;
}
bool _FullReset(void* self)
{
	struct c_class *this = self;
	if (this->Positions > 0)
	{
		Byte num = (this->Positions % 8) > 0 ? this->Positions / 8 + 1 : this->Positions / 8;
		for (int i = 0; i < num; i++)
			this->BitArray[i] = 0;
		return true;
	}
	return false;
}

constructMethodTable(.Any = _Any,.IsSet = _IsSet,.Set = _Set,.Reset = _Reset,.FullReset = _FullReset);

Constructor(void* self, va_list* ap)
{
	struct c_class *this = self;
	assignMethodTable(this);

	if (ap != NULL)
	{
		
		this->Positions =  va_arg(*ap, DWord);
		Byte num = (this->Positions % 8) > 0 ? this->Positions / 8 + 1 : this->Positions / 8;
		this->BitArray = malloc(num); //Because we allocate bytes
		if (!_FullReset(this)) return NULL;
	}
	else
	{
		this->BitArray = malloc(BLOCKSZ);
		this->Positions = BLOCKSZ;
		this->BitArray[0] = 0;
	}
	return this;
}

Destructor(void* self)
{
	struct c_class *this = self;
	free(this->BitArray);
	return this;
}

ENDCLASSDESC