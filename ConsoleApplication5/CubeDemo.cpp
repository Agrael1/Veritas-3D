#include "VeritasMath.h"
#include "Class.h"
#include "CubeDemo.h"

void MultiplyMatrixVector(VMVECTOR *i, VMVECTOR *o, VMMATRIX *m)
{
	o->m128_f32[0] = i->m128_f32[0] * m->r[0].m128_f32[0] + i->m128_f32[1] * m->r[1].m128_f32[0] + i->m128_f32[2] * m->r[2].m128_f32[0] + m->r[3].m128_f32[0];
	o->m128_f32[1] = i->m128_f32[0] * m->r[0].m128_f32[1] + i->m128_f32[1] * m->r[1].m128_f32[1] + i->m128_f32[2] * m->r[2].m128_f32[1] + m->r[3].m128_f32[1];
	o->m128_f32[2] = i->m128_f32[0] * m->r[0].m128_f32[2] + i->m128_f32[1] * m->r[1].m128_f32[2] + i->m128_f32[2] * m->r[2].m128_f32[2] + m->r[3].m128_f32[2];
	float w = i->m128_f32[0] * m->r[0].m128_f32[3] + i->m128_f32[1] * m->r[1].m128_f32[3] + i->m128_f32[2] * m->r[2].m128_f32[3] + m->r[3].m128_f32[3];

	if (w != 0.0f)
	{
		o->m128_f32[0] /= w; o->m128_f32[1] /= w; o->m128_f32[2] /= w;
	}
}

bool virtual(OnUserCreate)(void* self)
{
	account(self);
	
	this->meshCube.p = malloc(12*sizeof(Triangle)); 
	memcpy(this->meshCube.p, (Triangle[]) {

			// SOUTH
		{ 0.0f, 0.0f, 0.0f,		0.0f, 1.0f, 0.0f,	1.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f,		1.0f, 1.0f, 0.0f,   1.0f, 0.0f, 0.0f },

			// EAST                                                      
		{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f },
		{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f },

			// NORTH                                                     
		{ 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f },
		{ 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f },

			// WEST                                                      
		{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f },

			// TOP                                                       
		{ 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f },
		{ 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f },

			// BOTTOM                                                    
		{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f },

	}, 12 * sizeof(Triangle));
	
	// Setting up projection matrix and camera
	base.Output->projection = VMMatrixPerspectiveLH(1.0f, (float)base.Output->nFrameHeight / (float)base.Output->nFrameLength, 0.5f, 40.0f);
	base.Output->camera = VMMatrixTranslation(0.0f, 0.0f, 5.0f);

	return true;
}
bool virtual(OnUserUpdate)(void* self, double fElapsedSeconds)
{
	account(self);
	base.Output->method->ClearFrame(base.Output, 0, BG_BLACK);

	VMMATRIX rotZ = { 0 }, rotX = { 0 };
	this->fTheta += (float)fElapsedSeconds;

	// Calculate rotation matrix
	rotZ = VMMatrixRotationRollPitchYaw(-this->fTheta, 0, this->fTheta);

	for (UINT i = 0; i < 12; i++)
	{
		VMVECTOR a = VMLoadFloat3(&this->meshCube.p[i].p[0]);
		VMVECTOR b = VMLoadFloat3(&this->meshCube.p[i].p[1]);
		VMVECTOR c = VMLoadFloat3(&this->meshCube.p[i].p[2]);

		VMMATRIX proj = VMMatrixMultiply(VMMatrixMultiply(rotZ, &base.Output->camera),&base.Output->projection);

		// Rotate in Z-Axis and project
		a = VMVector3TransformCoord(a, proj);
		b = VMVector3TransformCoord(b, proj);
		c = VMVector3TransformCoord(c, proj);

		// Scale into view
		a.m128_f32[0] += 1.0f; a.m128_f32[1] += 1.0f;
		b.m128_f32[0] += 1.0f; b.m128_f32[1] += 1.0f;
		c.m128_f32[0] += 1.0f; c.m128_f32[1] += 1.0f;

		a.m128_f32[0] *= 0.5f * (float)base.Output->nFrameLength;
		a.m128_f32[1] *= 0.5f * (float)base.Output->nFrameHeight;
		b.m128_f32[0] *= 0.5f * (float)base.Output->nFrameLength;
		b.m128_f32[1] *= 0.5f * (float)base.Output->nFrameHeight;
		c.m128_f32[0] *= 0.5f * (float)base.Output->nFrameLength;
		c.m128_f32[1] *= 0.5f * (float)base.Output->nFrameHeight;

		// Rasterize triangle
		base.Output->method->DrawTriangle(base.Output,
			a.m128_f32[0], a.m128_f32[1],
			b.m128_f32[0], b.m128_f32[1],
			c.m128_f32[0], c.m128_f32[1],
			0x2588, FG_WHITE);
	}
	return true;
}
bool virtual(OnUserDestroy)(void* self)
{
	account(self);
	if (this->meshCube.p)
		free(this->meshCube.p);

	return true;
}

Constructor(void* self, va_list *ap)
{
	struct c_class *this = ((struct Class*)VeritasEngine)->ctor(self, ap);
	base.AppName = stringOf(self);
	base.method->OnUserCreate = virtual(OnUserCreate);
	base.method->OnUserUpdate = virtual(OnUserUpdate);
	base.method->OnUserDestroy = virtual(OnUserDestroy);
	return this;
}
Destructor(void* self)
{
	struct c_class *this = ((struct Class*)VeritasEngine)->dtor(self);

	return this;
}
ENDCLASSDESC