#include "game.h"

#include <SDL3_image/SDL_image.h>
#include <fstream>

#include "texture.h"
#include "GameObject.h"
#include "Button.h"
#include "Hover.h"
#include "SceneManager.h"

using namespace std;

constexpr const char* const WINDOW_TITLE = "Manuscrito Wolmir";

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
	{"Runas.JPEG"},
	{"TablaRunas.JPEG"},
	{"Selector.png"},
	{"Marco.PNG"},
	{"Hoja1.JPEG"},
	{"Hoja5.JPEG"},
	{"Hoja6.JPEG"},
	{"Hoja8.JPEG"},
	{"Folio.PNG"},
	{"HojaVacia.png"}
};

// FUENTES
TTF_Font* Game::baseFont = nullptr;
TTF_Font* Game::baseFontCentered = nullptr;
TTF_Font* Game::manuscritoFont = nullptr;

// OBJETOS DEL JUEGO
vector<GameObject*> Game::gameObjects;

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
	baseFont = TTF_OpenFont(((string)fontBase + "OldNewspaperTypes.ttf").c_str(), FONT_SIZE);
	if (!baseFont) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't load font");
	}
	TTF_SetFontWrapAlignment(baseFont, TTF_HORIZONTAL_ALIGN_LEFT);

	baseFontCentered = TTF_OpenFont(((string)fontBase + "OldNewspaperTypes.ttf").c_str(), FONT_SIZE);
	if (!baseFontCentered) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't load font");
	}
	TTF_SetFontWrapAlignment(baseFontCentered, TTF_HORIZONTAL_ALIGN_CENTER);

	manuscritoFont = TTF_OpenFont(((string)fontBase + "ManuscritoWolmir2.ttf").c_str(), MANUS_FONT_SIZE);
	if (!manuscritoFont) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't load font");
	}

	// Carga las texturas al inicio
	for (size_t i = 0; i < textures.size(); i++) {
		auto [name, nrows, ncols] = textureList[i];
		textures[i] = new Texture(renderer, (string(imgBase) + name).c_str(), nrows, ncols);
	}

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_MUL);
	SDL_SetRenderLogicalPresentation(renderer, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_STRETCH);
}

Game::~Game()
{
	TTF_CloseFont(baseFont);
	baseFont = nullptr;

	// Elimina los objetos del juego
	for (size_t i = 0; i < gameObjects.size(); i++) {
		delete gameObjects[i];
	}

	// Elimina las texturas
	for (size_t i = 0; i < textures.size(); i++) {
		delete textures[i];
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_Quit();
	SDL_Quit();
}

void
Game::render() const
{
	SceneManager::activeScene->Render();
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

	SceneManager::activeScene->Update(deltaTime);
}

void
Game::run()
{
	const Uint32 frameDelay = 1000 / FRAME_RATE;
	Uint32 frameStart;
	Uint32 frameTime;

	SceneManager::activeScene->Start();

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

		if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
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

		if (event.type == SDL_EVENT_MOUSE_MOTION) {
			float cur_posX, cur_posY;
			SDL_RenderCoordinatesFromWindow(renderer, event.motion.x, event.motion.y, &cur_posX, &cur_posY);

			for (GameObject* go : gameObjects) {
				if (go->getIsActive()) {
					Hover* h = go->getComponent<Hover>();

					if (h != nullptr && h->isEnabled) {
						Transform* t = go->getComponent<Transform>();
						SDL_FPoint cur_pos = { cur_posX, cur_posY };

						if (SDL_PointInRectFloat(&cur_pos, &t->dstRect)) {
							h->onHover();
						}
					}
				}
			}
		}

		if (event.type == SDL_EVENT_KEY_DOWN) {
			// SALIR DEL JUEGO
			if (event.key.key == SDLK_ESCAPE) {
				exit = true;
			}
		}

		SceneManager::activeScene->HandleEvents(event);
	}
}
