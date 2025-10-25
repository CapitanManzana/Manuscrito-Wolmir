#include "Scene.h"

int Scene::totalScenes = 0;
std::vector<GameObject*> sceneObjects;

Scene::Scene(Game& game) {
	this->game = &game;
	renderer = game.getRenderer();
	sceneID = totalScenes;
	totalScenes++;
}