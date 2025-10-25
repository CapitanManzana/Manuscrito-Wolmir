#include "game.h"
#include <string>
#include "MainGame.h"
#include "SceneManager.h"

int main(int argc, char* argv[])
{
	try {
		Game* game = new Game();

		// Creamos las escenas
		MainGame* mainGame = new MainGame(*game);
		mainGame->Load();

		SceneManager::addScene(mainGame);

		game->run();

		SceneManager::unload();
		delete game;
	}
	catch (const std::runtime_error& e) {
		SDL_Log("Se produjo un error: %s", e.what());
	}

	return 0;
}
