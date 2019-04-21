#include <stdio.h>
#include "GameDemo.h"
#include "VeritasEngine.h"
#include "Exception.h"

int main(void)
{
	struct VeritasEngine *VE;
	try
	{
		VE = new(VeritasEngine);
		VE->method->SetupScreen(VE, 320, 160, 8, 8);
	}
	catch (Exception, e)
	{
		MessageBoxA(NULL, e->method->what(e), e->method->GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	endtry

	if(VE)
		delete(VE);

	return 0;

	/*struct GameDemo *GD;
	try 
	{
		GD = new(GameDemo);

		if(GD->_.method->ConstructConsole(GD, 320, 200, 4, 4))
			GD->_.method->Start(GD);
	}
	catch (Exception, e)
	{
		e->method->what(e);
		printf("%s", e->whatBuffer);
	}
	endtry

	if (GD)
		delete(GD);*/

}