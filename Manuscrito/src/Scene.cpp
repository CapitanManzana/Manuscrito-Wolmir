#include "Scene.h"
#include "GameObject.h"
#include "SceneManager.h"

int Scene::totalScenes = 0;

Scene::Scene(Game& game) {
	this->game = &game;
	renderer = game.getRenderer();
	sceneID = totalScenes;
	totalScenes++;

	SceneManager::addScene(this);
}

void Scene::Start() {
	for (GameObject* g : sceneObjects) {
		g->start();
	}
}

void Scene::Update(float deltaTime) {
	for (GameObject* g : sceneObjects) {
		if (g->getIsActive()) {
			g->update(deltaTime);
		}
	}
}

void Scene::Render() {
	for (GameObject* g : sceneObjects) {
		if (g->getIsActive() && g->spriteRenderer != nullptr && g->spriteRenderer->isEnabled) {
			g->render();
		}
	}
}