#pragma once
#include "game.h"
#include "AudioManager.h"

class Scene
{
private:
	int sceneID;
	static int totalScenes;

protected:
	Game* game;
	SDL_Renderer* renderer;

public:
	Scene(Game& game);
	virtual ~Scene() {}

	std::vector<GameObject*> sceneObjects;

	virtual void Load() {}
	virtual void Reload() {}
	virtual void HandleEvents(SDL_Event& event) {}

	virtual void Update(float deltaTime);
	virtual void Start();
	virtual void Render();
};

