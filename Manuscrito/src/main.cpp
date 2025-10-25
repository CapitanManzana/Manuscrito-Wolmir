#include "game.h"
#include <string>
#include "MainGame.h"
#include "IntroScene.h"
#include "SceneManager.h"

int main(int argc, char* argv[])
{
	try {
		Game* game = new Game();

		// Creamos las escenas
		IntroScene* introScene = new IntroScene(*game);
		introScene->Load();

		MainGame* mainGame = new MainGame(*game);
		mainGame->Load();

		game->run();

		SceneManager::unload();
		delete game;
	}
	catch (const std::runtime_error& e) {
		SDL_Log("Se produjo un error: %s", e.what());
	}

	return 0;
}
