#include "Application3D.h"

int main() {
	
	// Allocate memory for the application
	auto app = new Application3D();

	// Initialise and run the application loop
	app->run("Real-Time 3D OpenGL Application - LowPoly Pirate Ship", 1280, 720, false);

	// Deallocate memory before exiting
	delete app;

	return 0;
}