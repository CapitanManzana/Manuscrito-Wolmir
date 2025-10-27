#include "SplashScreen.h"

#include "SceneManager.h"
#include "GameObject.h"
#include "Text.h"
#include "Fader.h"
#include "Hover.h"
#include "AudioManager.h"

void SplashScreen::Load() {
	logo = new GameObject("ContinueButton", 5);
	logo->addComponent<Transform>(Vector2D<float>(Game::WINDOW_WIDTH / 2, Game::WINDOW_HEIGHT / 2), 0.2);
	logo->addComponent<SpriteRenderer>(game->getTexture(Game::LOGO), 0, 0);
	Fader* f = logo->addComponent<Fader>(logoDuration, true);
	SceneType scene = MAIN_MENU;
	f->onFadeOutEnd = [scene]() { SceneManager::changeScene(scene); };
	f->onFadeInEnd = [f]() { f->startFadeOut(); };

	game->gameObjects.push_back(logo);
	sceneObjects.push_back(logo);

}

void SplashScreen::OnLoadScene() {
	AudioManager::playSong(AudioManager::MENU);
}