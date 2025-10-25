#pragma once
#include "game.h"

class Scene
{
private:
	int sceneID;
	static int totalScenes;

protected:
	std::vector<GameObject*> sceneObjects;
	Game* game;
	SDL_Renderer* renderer;

public:
	Scene(Game& game);
	virtual ~Scene() {}

	virtual void Load() {}
	virtual void Update(float deltaTime) {}
	virtual void Start() {}
	virtual void HandleEvents(SDL_Event& event) {}
	virtual void Render() {}
};

