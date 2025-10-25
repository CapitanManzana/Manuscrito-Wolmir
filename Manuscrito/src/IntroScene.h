#pragma once
#include "Scene.h"

class IntroScene : public Scene
{
public:
	IntroScene(Game& game) : Scene(game) { }

	void Load() override;
	void HandleEvents(SDL_Event& event) override;
};

