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