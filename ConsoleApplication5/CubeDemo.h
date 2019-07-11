#pragma once
#include "VeritasEngine.h"
#include "VTypes.h"
#include "IndexedTriangleList.h"
#include "EngineCommons.h"

#define c_class CubeDemo

class 
{
	inherits(VeritasEngine);
	
	float fTheta;

	VMVECTOR ScreenOffset;
	VMVECTOR ScreenResolution;
	
	bool bStop;
	UINT cycle, back, fore;
	struct IndexedTriangleList model;
};