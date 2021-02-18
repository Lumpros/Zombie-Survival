#include "Application.h"

int main(int argc, char* argv[])
{
	PZS::Application app;
	
	if (!app.Initialize())
		puts("Failed to Initialize application!");
	else
		app.Loop();

	return 0;
}