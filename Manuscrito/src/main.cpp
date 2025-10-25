#include "game.h"
#include <string>
#include "MainGame.h"
#include "IntroScene.h"
#include "SceneManager.h"
#include "LiberarScene.h"
#include "CerrarScene.h"
#include "SplashScreen.h"

int main(int argc, char* argv[])
{
	try {
		Game* game = new Game();

		// Creamos las escenas
		SplashScreen* splashScene = new SplashScreen(*game);
		splashScene->Load();

		IntroScene* introScene = new IntroScene(*game);
		introScene->Load();

		MainGame* mainGame = new MainGame(*game);
		mainGame->Load();

		LiberarScene* liberarScene = new LiberarScene(*game);
		liberarScene->Load();

		CerrarScene* cerrarScene = new CerrarScene(*game);
		cerrarScene->Load();

		SceneManager::changeScene(MAIN_GAME);
		game->run();

		SceneManager::unload();
		delete game;
	}
	catch (const std::runtime_error& e) {
		SDL_Log("Se produjo un error: %s", e.what());
	}

	return 0;
}
