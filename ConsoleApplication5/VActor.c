#include "CubeSceneTex.h"
#include "Camera.h"
#include "VActor.h"
#include "Class.h"

VMMATRIX _GetTransformVM(const selfptr)
{	
	VMMATRIX M1 = VMMatrixTranslationFromVector(VMLoadFloat3A(&self->WorldPosition));
	VMMATRIX M2 = VMMatrixRotationRollPitchYaw(0.0f, self->ACamera->theta, 0.0f);

	VMMATRIX M = VMMatrixMultiply(M2,&M1);
	return M;
}
void virtual(Reset)(selfptr)
{
	self->WorldPosition = (VMFLOAT3A) { 0.0f, 0.0f, 0.0f };
}
void virtual(Move)(selfptr, VMFLOAT3A dPosition)
{
	VMVECTOR R = VMLoadFloat3A(&dPosition);
	R = VMVector3Transform(
		R,
		VMMatrixScale(VMMatrixRotationRollPitchYaw( 0.0f,self->ACamera->theta, 0.0f), self->movespeed));
	VMStoreFloat3A(&self->WorldPosition, VMVectorAdd(R, VMLoadFloat3A(&self->WorldPosition.x)));
}

constructMethodTable(
	.Move = virtual(Move),
	.GetTransformVM = _GetTransformVM
);

Constructor(selfptr, va_list *ap)
{
	account(self);
	assignMethodTable(self);
	self->Mesh = new(CubeTex, 
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.9f);
	Reset_VActor(self);
	self->ACamera = new(Camera, &self->WorldPosition);
	self->movespeed = 2.0f;
	return self;
}
Destructor(selfptr)
{
	delete(self->Mesh);
	return self;
}
ENDCLASSDESC