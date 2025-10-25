#pragma once
#include "game.h"
#include <vector>

class Scene : public Game
{
private:
	int sceneID;
	static int totalScenes;

protected:
	std::vector<GameObject*> sceneObjects;

public:
	Scene() {
		sceneID = totalScenes;
		totalScenes++;
	}
	virtual ~Scene();

	virtual void Load();
	virtual void Update(float deltaTime);
	virtual void Start();
	virtual void HandleEvents(SDL_Event& event);
	virtual void Render();
};

