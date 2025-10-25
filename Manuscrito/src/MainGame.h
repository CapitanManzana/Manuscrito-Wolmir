#pragma once
#include "Scene.h"
#include <vector>
#include "vector2D.h"

class RunicTest;
class CodeTest;
class Book;
class Notebook;
class GameObject;

using namespace std;


class MainGame : public Scene 
{
private:
	// MANUSCRITO
	Book* manuscrito;
	int currentPage = 0;
	int pagesCount = 0;

	//Cuaderno de notas
	GameObject* notebookParent;
	Notebook* notebook;

	// TEXTOS
	GameObject* currentText = nullptr;

	// OVERLAY
	vector<GameObject*> overlays;

	// TESTS
	RunicTest* runicTest;
	RunicTest* astrologyTest;
	CodeTest* finalCodeTest;

	// HERAMIENTAS
	bool blackLight = false;
	bool paperNotes = false;

	// Mascara de luz uv
	SDL_Texture* light_tex;
	SDL_Texture* rendertex_light;
	SDL_Texture* rendertex;
	SDL_FRect rectLight;

	vector<GameObject*> uvObjects;

	float cur_posX;
	float cur_posY;

	// Luz ambiente
	SDL_Color ambientLightDarkZone = { 24, 0, 115, 225 };
	SDL_Color ambientLightIlumZone = { 212, 135, 51, 100 };
	SDL_Texture* ambientLight_tex;
	SDL_Texture* rendertex_ambientLight;
	SDL_Texture* rendertexLight;
	SDL_FRect rect_ambientLight;

	float lightRadius = 2.3f;
	Vector2D<float> lightPosition = Vector2D<float>(-375, -580);

	const char* const notesData = "../assets/data/notes.txt";
	const char* const textsData = "../assets/data/texts.txt";

public:
	MainGame() = default;
	virtual ~MainGame() = default;

	void Load() override;
	void Update(float deltaTime) override;
	void Start() override;
	void HandleEvents(SDL_Event& event) override;
	void Render() override;

private:
	void showText(GameObject* text);
	void renderObjects() const;

	void createGameObjects();
	void createUvLight();

	static vector<GameObject*> texts;
};

