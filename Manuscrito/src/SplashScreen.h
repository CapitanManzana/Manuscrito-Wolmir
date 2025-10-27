#pragma once
#include "Scene.h"

class GameObject;

class SplashScreen : public Scene
{
private:
	float logoDuration = 4.0f;
	GameObject* logo = nullptr;
	bool end = false;
public:
	SplashScreen(Game& game) : Scene(game) { }
	void Load() override;
	void OnLoadScene() override;
};

