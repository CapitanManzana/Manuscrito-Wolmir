#pragma once
#include "Scene.h"
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
	Book* manuscrito = nullptr;
	int currentPage = 0;
	int pagesCount = 0;

	//Cuaderno de notas
	GameObject* notebookParent = nullptr;
	Notebook* notebook = nullptr;

	// TEXTOS
	GameObject* currentText = nullptr;

	// OVERLAY
	vector<GameObject*> overlays;

	// TESTS
	RunicTest* runicTest = nullptr;
	RunicTest* astrologyTest = nullptr;
	CodeTest* finalCodeTest = nullptr;

	// HERAMIENTAS
	bool blackLight = false;
	bool paperNotes = false;

	// Mascara de luz uv
	SDL_Texture* light_tex = nullptr;
	SDL_Texture* rendertex_light = nullptr;
	SDL_Texture* rendertex = nullptr;
	SDL_FRect rectLight;

	vector<GameObject*> uvObjects;

	float cur_posX = 0;
	float cur_posY = 0;

	// Luz ambiente
	SDL_Color ambientLightDarkZone = { 24, 0, 115, 225 };
	SDL_Color ambientLightIlumZone = { 212, 135, 51, 100 };
	SDL_Texture* ambientLight_tex = nullptr;
	SDL_Texture* rendertex_ambientLight = nullptr;
	SDL_Texture* rendertexLight = nullptr;
	SDL_FRect rect_ambientLight;

	float lightRadius = 2.3f;
	Vector2D<float> lightPosition = Vector2D<float>(-375, -580);

	const char* const notesData = "../assets/data/notes.txt";
	const char* const textsData = "../assets/data/texts.txt";

public:
	MainGame(Game& game) : Scene(game) { }
	~MainGame() override;

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

