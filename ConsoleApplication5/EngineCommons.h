// EngineCommons.h
//
//		GNU GPL License 3.0. Usage or modification is strictly welcome.
//
// Main Inclusion file for all the classes in olcGameEngine project. 
// Defines all the crucial automation for class mechanics in ANSI-C and some data types
// Designed for multiple inclusions, does not consume memory, contains only compiler instructions.
// Usage: include last to every class declaration header.
#ifndef ENGINECOMMONS_H
#define ENGINECOMMONS_H

#include "New.h"

typedef unsigned char Byte;
typedef _Bool bool;
typedef unsigned short Word;
typedef unsigned int DWord;

#if _WIN64
typedef unsigned long long MaxInt;
#else
typedef unsingned int MaxInt;
#endif

#define M_PI 3.14159265358979323846
#define true 1
#define false 0

#pragma region Automation
// Easy mangling for method and private tables
#define __xvftb(x) x ## _vftb
#define __rvftb(x) __xvftb(x)
#define vftb __rvftb(c_class)

#define __xprivate(x) x ## _private
#define __rprivate(x) __xprivate(x)
#define _private __rprivate(c_class)

#define __xmeth(x) x ## _method
#define __rmeth(x) __xmeth(x)
#define meth __rmeth(c_class)

#define __rclass(x) extern const void* x; struct x
#define class __rclass(c_class)

#define __xctor(x) x ## _ctor
#define __rctor(x) __xctor(x)
#define Constructor void* __rctor(c_class)

#define __xdtor(x) x ## _dtor
#define __rdtor(x) __xdtor(x)
#define Destructor void* __rdtor(c_class)

#define __xctab(x) _ ## x
#define __rctab(x) __xctab(x)
#define ctab __rctab(c_class)

#define __xconcat(x,y) x##y
#define __rconcat(x,y) __xconcat(x,y)


#pragma endregion

#define virtual(x) __rconcat( x, __rctab(c_class))
// Private Handling
#define privatev(...) Byte __internal_prtb[ sizeof( struct _private{ __VA_ARGS__ } )]
#define private (*(struct _private*)(this->__internal_prtb))
// Method Handling
#define methods(...) struct vftb { __VA_ARGS__ }*method
#define constructMethodTable(...) struct vftb meth = { __VA_ARGS__ }
#define assignMethodTable(x) (x)->method = &meth

// Class construction handling
#define ENDCLASSDESC const struct Class ctab = { sizeof(struct c_class),.ctor = __rctor(c_class),.dtor = __rdtor(c_class) }; \
const void* c_class = &ctab;
#endif

#undef c_class

 



