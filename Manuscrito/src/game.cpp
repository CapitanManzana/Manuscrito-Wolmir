#include "game.h"

#include <SDL3_image/SDL_image.h>
#include "texture.h"
#include "GameObject.h"
#include "Button.h"
#include "Book.h"

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
	TextureSpec{"fondo.jpg"},
	{"chino.jpeg"},
	{"Hoja1.png"},
	{"hoja3.png"},
	{"hoja4.png"},
	{"hoja5.png"},
	{"HojaVacia.png"}
};

vector<GameObject*> Game::gameObjects;
Book* manuscrito;
int currentPage = 0;
int pagesCount = 0;

bool blackLight = false;

Game::Game() : exit(false)
{
#pragma region SDL INIT
	// Carga SDL y sus bibliotecas auxiliares
	SDL_Init(SDL_INIT_VIDEO);
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

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_MUL);
	SDL_SetRenderLogicalPresentation(renderer, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_STRETCH);
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

	delete manuscrito;

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

	if (blackLight) {
		SDL_SetRenderDrawColor(renderer, 24, 0, 115, 160);
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

		/*else if (event.type == SDL_EVENT_WINDOW_RESIZED) {
			int w = event.window.data1;
			int h = event.window.data2;

			for (GameObject* go : gameObjects) {
				Transform* t = go->getComponent<Transform>();

				if (t != nullptr) {
					t->setPosition(Vector2D<float>(w / 2 - t->getScale().x / 2,
						h / 2 - t->getScale().y / 2));
				}
			}
		}*/

		else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
			float mouseX = event.button.x;
			float mouseY = event.button.y;

			for (GameObject* go : gameObjects) {
				if (go->getIsActive()) {
					Button* bt = go->getComponent<Button>();

					if (bt != nullptr) {
						Transform* t = go->getComponent<Transform>();

						if (t != nullptr && t->overlapingPoint(Vector2D<float>(mouseX, mouseY))) {
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

			if (event.key.key == SDLK_D) {
				currentPage+=2;
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
	int w;
	int h;

	SDL_GetWindowSize(window, &w, &h);
	vector<GameObject*> bookPages;
	// TODO
#pragma region CHINO

	/*GameObject* chino = new GameObject("Chino", 3);
	chino->addComponent<Transform>(Vector2D<float>(w / 2 + 150, h / 2 - 600), Vector2D<float>(1, 1));
	chino->addComponent<SpriteRenderer>(getTexture(CHINO), 0, 0);

	Button* chinoButton = chino->addComponent<Button>();

	chinoButton->onClick = [chino]() {
		currentPage++;
		if (currentPage >= pagesCount - 1) currentPage = 0;

		manuscrito->changePage(currentPage);
	};

	gameObjects.push_back(chino);*/

#pragma endregion

	GameObject* hoja1 = new GameObject("Hoja1", 2);
	hoja1->addComponent<Transform>(Vector2D<float>(w / 2 - 400, h / 2), 0.375);
	hoja1->addComponent<SpriteRenderer>(getTexture(HOJA1), 0, 0);

	GameObject* hoja2 = new GameObject("Hoja2", 2);
	hoja2->addComponent<Transform>(Vector2D<float>(w / 2 + 400, h / 2), 0.375);
	hoja2->addComponent<SpriteRenderer>(getTexture(HOJA1), 0, 0);

	GameObject* hoja3 = new GameObject("Hoja3", 2);
	hoja3->addComponent<Transform>(Vector2D<float>(w / 2 + 400, h / 2), Vector2D<float>(1, 1));
	hoja3->addComponent<SpriteRenderer>(getTexture(HOJA3), 0, 0);

	GameObject* hoja4 = new GameObject("Hoja4", 2);
	hoja4->addComponent<Transform>(Vector2D<float>(w / 2 + 400, h / 2), Vector2D<float>(1, 1));
	hoja4->addComponent<SpriteRenderer>(getTexture(HOJA4), 0, 0);

	GameObject* hoja5 = new GameObject("Hoja5", 2);
	hoja5->addComponent<Transform>(Vector2D<float>(w / 2 + 400, h / 2), Vector2D<float>(1, 1));
	hoja5->addComponent<SpriteRenderer>(getTexture(HOJA5), 0, 0);

	gameObjects.push_back(hoja1);
	gameObjects.push_back(hoja2);
	gameObjects.push_back(hoja3);
	gameObjects.push_back(hoja4);
	gameObjects.push_back(hoja5);

	bookPages.push_back(hoja1);
	bookPages.push_back(hoja2);
	bookPages.push_back(hoja3);
	bookPages.push_back(hoja4);
	bookPages.push_back(hoja5);

	manuscrito = new Book(bookPages, WINDOW_WIDTH / 2 - 5, WINDOW_HEIGHT / 2 - 9, 125);
	pagesCount = manuscrito->getPageCount();
}