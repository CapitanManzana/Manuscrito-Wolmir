#include "game.h"

#include <SDL3_image/SDL_image.h>
#include "texture.h"
#include "GameObject.h"

using namespace std;

// Constantes
constexpr const char* const WINDOW_TITLE = "Manuscrito";

// Estructura para especificar las texturas que hay que
// cargar y el tamaño de su matriz de frames
struct TextureSpec
{
	const char* name;
	int nrows = 1;
	int ncols = 1;
};

constexpr const char* const imgBase = "../assets/images/";

constexpr array<TextureSpec, Game::NUM_TEXTURES> textureList{
	TextureSpec{"background.jpg"},
	{"chino.jpeg"}
};

vector<GameObject*> Game::gameObjects;

Game::Game() : exit(false)
{
#pragma region SDL INIT
	// Carga SDL y sus bibliotecas auxiliares
	SDL_Init(SDL_INIT_VIDEO);
	//SDL_WINDOW_FULLSCREEN
	window = SDL_CreateWindow(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);

	if (window == nullptr)
		throw "window: "s + SDL_GetError();

	renderer = SDL_CreateRenderer(window, nullptr);

	if (renderer == nullptr)
		throw "renderer: "s + SDL_GetError();
#pragma endregion

	// Inicializamos las variables de tiempo
	perfFrequency = SDL_GetPerformanceFrequency();
	lastTime = SDL_GetPerformanceCounter();

	// Carga las texturas al inicio
	for (size_t i = 0; i < textures.size(); i++) {
		try {
			auto [name, nrows, ncols] = textureList[i];
			textures[i] = new Texture(renderer, (string(imgBase) + name).c_str(), nrows, ncols);
		}
		catch (const std::runtime_error& e) {
			SDL_Log("Se produjo un error: %s", e.what());
		}
	}

	//Carga los GameObjects

	createGameObjects();

	// Configura que se pueden utilizar capas translúcidas
	// SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
}

Game::~Game()
{
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
	SDL_Quit();
}

void
Game::render() const
{
	SDL_RenderClear(renderer);

	getTexture(BACKGROUND)->render();

	for (size_t i = 0; i < gameObjects.size(); i++) {
		if (gameObjects[i]->getIsActive())
			gameObjects[i]->render();
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
	while (!exit) {
		handleEvents();
		update();
		render();
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

		else if (event.type == SDL_EVENT_WINDOW_RESIZED) {
			int w = event.window.data1;
			int h = event.window.data2;

			for (GameObject* go : gameObjects) {
				Transform* t = go->getComponent<Transform>();

				if (t != nullptr) {
					t->setPosition(Vector2D<float>(w / 2 - t->scale.x / 2,
						h / 2 - t->scale.y / 2));
				}
			}
		}

		else if (event.type == SDL_EVENT_KEY_DOWN) {

			if (event.key.key == SDLK_E) {
				gameObjects[0]->setIsActive(!gameObjects[0]->getIsActive());
			}
		}

		// TODO
	}
}

bool
Game::checkCollision(const SDL_FRect& rect) const
{
	// TODO: cambiar el tipo de retorno a Collision e implementar
	return false;
}

void Game::createGameObjects() {
	// TODO
	Transform* chinoTransform = new Transform(
		Vector2D<float>(WINDOW_WIDTH / 2 - 150, WINDOW_HEIGHT / 2 - 150),
		Vector2D<float>(300, 300)
	);
	SpriteRenderer* chinoSprite = new SpriteRenderer(getTexture(CHINO), 0, 0);

	GameObject* chino = new GameObject("Chino", chinoTransform, chinoSprite, 2);

	Game::gameObjects.push_back(chino);
}