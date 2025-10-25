#pragma once
#include "Scene.h"

enum SceneType {
	INTRO = 0,
	MAIN_GAME,
	FINAL_LIBERAR,
	FINAL_CERRAR
};

static class SceneManager
{
public:
	static void unload();

	static Scene* activeScene;

	static void changeScene(SceneType scene);
	static void addScene(Scene* scene);

	static std::vector<Scene*> scenes;
};

