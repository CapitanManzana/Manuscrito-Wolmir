#pragma once
#include "Scene.h"

enum SceneType {
	NONE = -1,
	SPLASH_SCREEN,
	MAIN_MENU,
	INTRO,
	MAIN_GAME,
	FINAL_LIBERAR,
	FINAL_CERRAR
};

static class SceneManager
{
public:
	static void unload();

	static Scene* activeScene;
	static SceneType scene;
	static SceneType nextScene;

	static void changeScene(SceneType scene);
	static void addScene(Scene* scene);
	static void applySceneChange();

	static SceneType getActiveScene();

	static std::vector<Scene*> scenes;
};

