#include "game.h"
#include <string>

int main(int argc, char* argv[])
{
	try {
		Game().run();
	}
	catch (const std::runtime_error& e) {
		SDL_Log("Se produjo un error: %s", e.what());
	}

	return 0;
}
