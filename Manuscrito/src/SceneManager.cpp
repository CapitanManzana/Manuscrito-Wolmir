#include "SceneManager.h"

Scene* SceneManager::activeScene;
SceneType SceneManager::nextScene = (SceneType)-1;
std::vector<Scene*> SceneManager::scenes;
SceneType SceneManager::scene;

void SceneManager::changeScene(SceneType scene) {
	SDL_Log("Cambiando a escena %i", scene);
	nextScene = scene;
}

void SceneManager::applySceneChange() {
	if (nextScene != (SceneType)-1) {
		SDL_Log("Aplicando cambio a escena %i", nextScene);
		activeScene = scenes[nextScene];
		activeScene->OnLoadScene();
		scene = nextScene;
		nextScene = (SceneType)-1; // resetea el marcador
	}
}

void SceneManager::addScene(Scene* scene) {
	if (std::find(scenes.begin(), scenes.end(), scene) == scenes.end()) {
		scenes.push_back(scene);
		if (scenes.size() == 1) {
			activeScene = scene;
		}
	}
	else {
		SDL_Log("La escena ya estaba contenida, no se ha añadido");
	}
}

void SceneManager::unload() {
	for (Scene* s : scenes) {
		delete s;
	}

	scenes.clear();
}

SceneType SceneManager::getActiveScene() {
	return scene;
}