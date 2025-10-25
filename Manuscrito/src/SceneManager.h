#pragma once
#include "Scene.h"

static class SceneManager
{
public:
	static void unload();

	static Scene* activeScene;

	static void changeScene(int index);
	static void addScene(Scene* scene);

	static std::vector<Scene*> scenes;
};

