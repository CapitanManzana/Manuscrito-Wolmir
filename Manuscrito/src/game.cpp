#include "game.h"

#include <SDL3/SDL_image.h>
#include <fstream>

#include "texture.h"
#include "GameObject.h"
#include "Button.h"
#include "Hover.h"
#include "SceneManager.h"
#include "AudioManager.h"

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
	{"Logo.png"},
	{"Fader.jpg"},
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

	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error al inicializar SDL: %s", SDL_GetError());
		exit = true;
	}

	if (!TTF_Init()) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize TTF");
		exit = true;
	}

	// Inicializa SDL_mixer (puedes especificar formatos como OGG, MP3)
	if (!MIX_Init()) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No se ha cargado SDL_mixer");
		exit = true;
	}

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

	AudioManager::Init();
}

Game::~Game()
{
	TTF_CloseFont(baseFont);
	TTF_CloseFont(manuscritoFont);
	TTF_CloseFont(baseFontCentered);
	baseFont = nullptr;
	manuscritoFont = nullptr;
	baseFontCentered = nullptr;

	AudioManager::Unload();

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
	SDL_RenderClear(renderer);

 	currentScene->Render();

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

	currentScene->Update(deltaTime);

	AudioManager::Update(deltaTime);
}

void
Game::run()
{
	currentScene = SceneManager::activeScene;

	const Uint32 frameDelay = 1000 / FRAME_RATE;
	Uint32 frameStart;
	Uint32 frameTime;

	currentScene->Start();

	// Bucle principal del juego
	while (!exit) {
		frameStart = SDL_GetTicks();

		handleEvents();
		update();
		render();

		prevScene = SceneManager::activeScene;
		SceneManager::applySceneChange();
		currentScene = SceneManager::activeScene;

		if (prevScene != currentScene) {
			prevScene->Reload(); //recarga la escena anterior
		}

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

			for (GameObject* go : currentScene->sceneObjects) {
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

		currentScene->HandleEvents(event);
	}
}
