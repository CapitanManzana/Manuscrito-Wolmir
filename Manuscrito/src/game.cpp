#include "game.h"

#include <SDL3_image/SDL_image.h>

#include "texture.h"
#include "GameObject.h"
#include "Button.h"
#include "Text.h"
#include "Book.h"

//#include <json.hpp>

using namespace std;

// Constantes
constexpr const char* const WINDOW_TITLE = "Manuscrito Wolmir";

// Estructura para especificar las texturas que hay que
// cargar y el tamaño de su matriz de frames
struct TextureSpec
{
	const char* name;
	int nrows = 1;
	int ncols = 1;
};

constexpr const char* const imgBase = "../assets/images/";
constexpr const char* const fontBase = "../assets/font/";

constexpr array<TextureSpec, Game::NUM_TEXTURES> textureList{
	TextureSpec{"fondo.jpg"},
	{"chino.jpeg"},
	{"Hoja1.png"},
	{"hoja3.png"},
	{"hoja4.png"},
	{"hoja5.png"},
	{"HojaVacia.png"},
	{"Texto1.1.png"}
};

vector<GameObject*> Game::gameObjects;
Book* manuscrito;
int currentPage = 0;
int pagesCount = 0;

vector<GameObject*> texts;

bool blackLight = false;
bool glasses = false;

Game::Game() : exit(false)
{
#pragma region SDL INIT
	// Carga SDL y sus bibliotecas auxiliares
	SDL_Init(SDL_INIT_VIDEO);
	if (!TTF_Init()) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize TTF");
	}

	//SDL_WINDOW_FULLSCREEN
	//SDL_WINDOW_RESIZABLE
	window = SDL_CreateWindow(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_FULLSCREEN);

	if (window == nullptr)
		throw "window: "s + SDL_GetError();

	renderer = SDL_CreateRenderer(window, nullptr);

	if (renderer == nullptr)
		throw "renderer: "s + SDL_GetError();
#pragma endregion

	// Inicializamos las variables de tiempo
	perfFrequency = SDL_GetPerformanceFrequency();
	lastTime = SDL_GetPerformanceCounter();

	//Carga la fuente
	font = TTF_OpenFont(((string)fontBase + "XTypewriter-Regular.ttf").c_str(), FONT_SIZE);
	if (!font) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't load font");
	}

	// Carga las texturas al inicio
	for (size_t i = 0; i < textures.size(); i++) {
		auto [name, nrows, ncols] = textureList[i];
		textures[i] = new Texture(renderer, (string(imgBase) + name).c_str(), nrows, ncols);
	}

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_MUL);
	SDL_SetRenderLogicalPresentation(renderer, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_STRETCH);

	//Carga los GameObjects
	createGameObjects();


	// Configura que se pueden utilizar capas translúcidas
	// SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
}

Game::~Game()
{
	TTF_CloseFont(font);
	font = nullptr;

	texts.clear();

	// Elimina los objetos del juego
	for (size_t i = 0; i < gameObjects.size(); i++) {
		delete gameObjects[i];
	}

	// Elimina las texturas
	for (size_t i = 0; i < textures.size(); i++) {
		delete textures[i];
	}

	delete manuscrito;

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_Quit();
	SDL_Quit();
}

void
Game::render() const
{
	SDL_RenderClear(renderer);

	getTexture(BACKGROUND)->render();

	for (size_t i = 0; i < gameObjects.size(); i++) {
		if (gameObjects[i]->getIsActive() && gameObjects[i]->spriteRenderer != nullptr && gameObjects[i]->spriteRenderer->isEnabled)
			gameObjects[i]->render();
	}

	if (blackLight) {
		SDL_SetRenderDrawColor(renderer, 24, 0, 115, 160);
		SDL_RenderFillRect(renderer, nullptr);
	}
	else if (glasses) {
		SDL_SetRenderDrawColor(renderer, 0, 115, 0, 160);
		SDL_RenderFillRect(renderer, nullptr);
	}



	SDL_RenderPresent(renderer);
}

void
Game::update()
{
	// 1. Obtenemos el tiempo actual
	Uint64 currentTime = SDL_GetPerformanceCounter();

	// 2. Calculamos el deltaTime en segundos
	//    (tiempo transcurrido) / (ticks por segundo)
	float deltaTime = (float)(currentTime - lastTime) / (float)perfFrequency;

	// 3. Actualizamos lastTime para el siguiente fotograma
	lastTime = currentTime;

	// 4. Ahora llamamos al update de todos los GameObjects, pasando el deltaTime
	for (size_t i = 0; i < gameObjects.size(); i++) {
		if (gameObjects[i]->getIsActive())
			gameObjects[i]->update(deltaTime); // Pasamos el valor calculado
	}
}

void
Game::run()
{
	const Uint32 frameDelay = 1000 / FRAME_RATE;
	Uint32 frameStart;
	Uint32 frameTime;

	// Bucle principal del juego
	while (!exit) {
		frameStart = SDL_GetTicks();

		handleEvents();
		update();
		render();

		frameTime = SDL_GetTicks() - frameStart;

		if (frameDelay > frameTime) {
			SDL_Delay(frameDelay - frameTime);
		}
	}
}

void
Game::handleEvents()
{
	SDL_Event event;

	// Only quit is handled directly, everything else is delegated
	while (SDL_PollEvent(&event)) {

		if (event.type == SDL_EVENT_QUIT)
			exit = true;

		else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
			float mouseX = event.button.x;
			float mouseY = event.button.y;

			float logicalX, logicalY;

			SDL_RenderCoordinatesFromWindow(renderer, mouseX, mouseY, &logicalX, &logicalY);
			SDL_FPoint mousePoint = { logicalX, logicalY };

			for (GameObject* go : gameObjects) {
				if (go->getIsActive()) {
					Button* bt = go->getComponent<Button>();

					if (bt != nullptr && bt->isEnabled) {
						Transform* t = go->getComponent<Transform>();

						if (SDL_PointInRectFloat(&mousePoint, &t->dstRect)) {
							bt->onClick();
						}
					}
				}
			}
		}

		else if (event.type == SDL_EVENT_KEY_DOWN) {

			if (event.key.key == SDLK_E) {
				blackLight = !blackLight;
			}

			if (event.key.key == SDLK_Q) {
				glasses = !glasses;
			}

			if (event.key.key == SDLK_D) {
				currentPage += 2;
				if (currentPage >= pagesCount - 1) currentPage = pagesCount - 2;

				manuscrito->changePage(currentPage);
			}

			if (event.key.key == SDLK_A) {
				currentPage = currentPage - 2;
				if (currentPage < 0) currentPage = 0;
				manuscrito->changePage(currentPage);
			}

			if (event.key.key == SDLK_ESCAPE) {
				exit = true;
			}
		}
	}
}

void Game::createGameObjects() {

	#pragma region HOJAS DEL MANUSCRITO

	vector<GameObject*> bookPages;

	GameObject* hoja1 = new GameObject("Hoja1", 2);
	hoja1->addComponent<Transform>(Vector2D<float>(1, 1), 0.1875);
	hoja1->addComponent<SpriteRenderer>(getTexture(HOJA1), 0, 0);

	GameObject* hoja2 = new GameObject("Hoja2", 2);
	hoja2->addComponent<Transform>(Vector2D<float>(1, 1), 0.1875);
	hoja2->addComponent<SpriteRenderer>(getTexture(HOJA1), 0, 0);

	GameObject* hoja3 = new GameObject("Hoja3", 2);
	hoja3->addComponent<Transform>(Vector2D<float>(1, 1), 0.1875);
	hoja3->addComponent<SpriteRenderer>(getTexture(HOJA_VACIA), 0, 0);

	gameObjects.push_back(hoja1);
	gameObjects.push_back(hoja2);
	gameObjects.push_back(hoja3);

	bookPages.push_back(hoja1);
	bookPages.push_back(hoja2);
	bookPages.push_back(hoja3);

	manuscrito = new Book(bookPages, WINDOW_WIDTH / 2 - 5, WINDOW_HEIGHT / 2 - 9, 125);
	pagesCount = manuscrito->getPageCount();
#pragma endregion


	GameObject* texto1 = new GameObject("Texto1", 4, hoja1);

	texto1->addComponent<Transform>(Vector2D<float>(-5, -120), 0.1875);
	texto1->addComponent<SpriteRenderer>();
	string texto = "En un lugar de la Mancha, de cuyo nombre no quiero acordarme...";
	texto1->addComponent<Text>(texto, SDL_Color{ 0, 0, 0, 255 }, font, FONT_SIZE, renderer);

	Button* btT1 = texto1->addComponent<Button>();
	btT1->onClick = [this, texto1]() { 
		showText(texto1); 
	};
	texto1->spriteRenderer->isEnabled = false;

	gameObjects.push_back(texto1);

	texts.push_back(texto1);
}

#pragma region ButtonEvents

void Game::showText(GameObject* text) {
	if (glasses) {

		if (text->spriteRenderer->isEnabled) {
			for (GameObject* t : texts) {
				if (t != text) t->spriteRenderer->isEnabled = false;
			}
		}

		text->spriteRenderer->isEnabled = !text->spriteRenderer->isEnabled;
	}
}
#pragma endregion
