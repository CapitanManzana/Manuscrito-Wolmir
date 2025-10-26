#pragma once
#include "Scene.h"
#include <vector>
#include "SceneManager.h"
#include "vector2D.h"

class Book;
class GameObject;

class MainMenu : public Scene
{
private:
	const char* const menuTextsData = "../assets/data/MainMenu.txt";

	Book* libro = nullptr;
	int currentPage = 0;
	int pagesCount = 0;

	float fadeTime = 3.0f;
	GameObject* fader = nullptr;
	std::vector<GameObject*> pages;

	// Luz ambiente
	SDL_Color ambientLightDarkZone = { 24, 0, 115, 225 };
	SDL_Color ambientLightIlumZone = { 212, 135, 51, 100 };
	SDL_Texture* ambientLight_tex = nullptr;
	SDL_Texture* rendertex_ambientLight = nullptr;
	SDL_Texture* rendertexLight = nullptr;
	SDL_FRect rect_ambientLight;

	float lightRadius = 1;
	Vector2D<float> lightPosition = Vector2D<float>(-250, -450);

public:
	MainMenu(Game& game) : Scene(game) {}
	~MainMenu() override;

	void Load() override;
	void Reload() override;
	void Render() override;
	void HandleEvents(SDL_Event& event) override;

private:
	void changeScene(SceneType scene);
};

