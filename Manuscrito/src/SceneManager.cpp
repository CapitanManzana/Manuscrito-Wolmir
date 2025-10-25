#include "SceneManager.h"

Scene* SceneManager::activeScene;
std::vector<Scene*> SceneManager::scenes;

void SceneManager::changeScene(SceneType scene) {
	activeScene = scenes[scene];
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
}