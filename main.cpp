#include "Application.h"

#ifdef main
	#undef main
#endif

int main(void)
{
	PZS::Application app;
	
	if (!app.Initialize())
		puts("Failed to Initialize application!");
	else
		app.Loop();

	return 0;
}