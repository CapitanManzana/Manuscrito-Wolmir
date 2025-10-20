#include "game.h"

#include <SDL3_image/SDL_image.h>

#include "texture.h"
#include "GameObject.h"
#include "Button.h"
#include "Text.h"
#include "Book.h"
#include "LoadTexts.h"
#include "Selector.h"
#include "RunicTest.h"

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
	{"Runas.JPEG"},
	{"TablaRunas.JPEG"},
	{"Selector.png"},
	{"Hoja1.JPEG"},
	{"HojaVacia.png"}
};

#pragma region VARIABLES

// OBJETOS DEL JUEGO
vector<GameObject*> Game::gameObjects;

// MANUSCRITO
vector<GameObject*> bookPages;
Book* manuscrito;
int currentPage = 0;
int pagesCount = 0;

// TEXTOS
vector<GameObject*> texts;
GameObject* currentText = nullptr;

// OVERLAY
vector<GameObject*> overlays;

// TESTS
RunicTest* runicTest;

// HERAMIENTAS
bool blackLight = false;
bool glasses = false;

// Mascara de luz uv
SDL_Texture* light_tex;
SDL_Texture* rendertex_light;
SDL_Texture* rendertex;
SDL_FRect rectLight;

float cur_posX;
float cur_posY;

#pragma endregion


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

	manuscritoFont = TTF_OpenFont(((string)fontBase + "ManuscritoWolmir2.ttf").c_str(), MANUS_FONT_SIZE) ;
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

	//Carga los GameObjects
	createGameObjects();

	SDL_Surface* lightSurf = IMG_Load("../assets/images/light.png");
	rectLight = { 0, 0, static_cast<float>(lightSurf->w), static_cast<float>(lightSurf->h) };
	light_tex = SDL_CreateTextureFromSurface(renderer, lightSurf);
	SDL_SetTextureBlendMode(light_tex, SDL_BLENDMODE_ADD);

	rendertex_light = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, WINDOW_WIDTH, WINDOW_HEIGHT);
	SDL_SetTextureBlendMode(rendertex_light, SDL_BLENDMODE_MOD);

	rendertex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, WINDOW_WIDTH, WINDOW_HEIGHT);
	SDL_SetTextureBlendMode(rendertex, SDL_BLENDMODE_MOD);

	SDL_DestroySurface(lightSurf);
}

Game::~Game()
{
	TTF_CloseFont(baseFont);
	baseFont = nullptr;

	texts.clear();

	delete runicTest;

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

	if (!blackLight) {
		renderObjects();

		if (glasses) {
			SDL_SetRenderDrawColor(renderer, 0, 115, 0, 20);
			SDL_RenderFillRect(renderer, nullptr);
		}
		else {
			SDL_SetRenderDrawColor(renderer, 0, 16, 48, 160);
			SDL_RenderFillRect(renderer, nullptr);
		}
	}
	else {
		rectLight.x = cur_posX - (rectLight.w / 2.f);
		rectLight.y = cur_posY - (rectLight.h / 2.f);

		// 1. PREPARAR LA MÁSCARA DE LUZ
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		// Apunta a tu textura de luz (que tiene BLENDMODE_MOD)
		SDL_SetRenderTarget(renderer, rendertex_light);
		// Limpia la textura de luz a negro (oscuridad total)
		SDL_RenderClear(renderer);
		// Renderiza el gradiente (light_tex, que tiene BLENDMODE_ADD) 
		// sobre la textura negra.
		SDL_RenderTexture(renderer, light_tex, NULL, &rectLight);

		// 2. RENDERIZAR LA ESCENA FINAL A LA PANTALLA

		// Apunta de nuevo a la pantalla
		SDL_SetRenderTarget(renderer, NULL);
		SDL_RenderClear(renderer); // Limpia la pantalla

		// Dibuja el mapa base
		renderObjects();

		SDL_SetRenderDrawColor(renderer, 24, 0, 115, 160);
		SDL_RenderFillRect(renderer, nullptr);

		// Multiplica el mapa por tu máscara de luz (rendertex_light)
		SDL_RenderTexture(renderer, rendertex_light, NULL, NULL);
	}

	// Muestra el resultado
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

	runicTest->DoUpdate(deltaTime);
}

void
Game::run()
{
	const Uint32 frameDelay = 1000 / FRAME_RATE;
	Uint32 frameStart;
	Uint32 frameTime;

	for (size_t i = 0; i < gameObjects.size(); i++) {
		gameObjects[i]->start();
	}

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
			SDL_RenderCoordinatesFromWindow(renderer, event.motion.x, event.motion.y, &cur_posX, &cur_posY);
		}

		if (event.type == SDL_EVENT_KEY_DOWN) {

			// LUZ UV
			if (event.key.key == SDLK_E) {
				blackLight = !blackLight;
			}

			// GAFAS DESCIFRADORAS
			if (event.key.key == SDLK_Q) {
				glasses = !glasses;
				if (currentText) {
					Text* t = currentText->getComponent<Text>();
					if (glasses) {
						t->changeFont(baseFont, t->getFontSize() - 16);
					}
					else {
						t->changeFont(manuscritoFont, t->getFontSize());
					}
				}
			}

			// CAMBIO DE PÁGINA DERECHA
			if (event.key.key == SDLK_D) {
				currentPage += 2;
				if (currentPage >= pagesCount) currentPage = pagesCount - 2;

				manuscrito->changePage(currentPage);
			}

			// CAMBIO DE PÁGINA IZQUIERDA
			if (event.key.key == SDLK_A) {
				currentPage -= 2;
				if (currentPage < 0) currentPage = 0;

				manuscrito->changePage(currentPage);
			}

			// SALIR DEL JUEGO
			if (event.key.key == SDLK_ESCAPE) {
				exit = true;
			}
		}
	}
}

// Se crean los objetos del juego
void Game::createGameObjects() {

#pragma region HOJAS DEL MANUSCRITO

	GameObject* hoja1 = new GameObject("Hoja1", 2);
	hoja1->addComponent<Transform>(Vector2D<float>(1, 1), 0.1875);
	hoja1->addComponent<SpriteRenderer>(getTexture(HOJA1), 0, 0);

	GameObject* hoja2 = new GameObject("Hoja2", 2);
	hoja2->addComponent<Transform>(Vector2D<float>(1, 1), 0.1875);
	hoja2->addComponent<SpriteRenderer>(getTexture(HOJA_VACIA), 0, 0);

	GameObject* hoja3 = new GameObject("Hoja3", 2);
	hoja3->addComponent<Transform>(Vector2D<float>(1, 1), 0.16);
	hoja3->addComponent<SpriteRenderer>(getTexture(TABLA_RUNAS), 0, 0);

	GameObject* hoja4 = new GameObject("Hoja4", 2);
	hoja4->addComponent<Transform>(Vector2D<float>(1, 1), 0.16);
	hoja4->addComponent<SpriteRenderer>(getTexture(RUNAS), 0, 0);

	bookPages.push_back(hoja1);
	bookPages.push_back(hoja2);
	bookPages.push_back(hoja3);
	bookPages.push_back(hoja4);

	manuscrito = new Book(bookPages, WINDOW_WIDTH / 2 - 5, WINDOW_HEIGHT / 2 - 9, 125);
	pagesCount = manuscrito->getPageCount();

	GameObject* hoja4_2 = new GameObject("Hoja4_2", 2);
	hoja4_2->addComponent<Transform>(Vector2D<float>(1, 1), 0.16);
	hoja4_2->addComponent<SpriteRenderer>(getTexture(HOJA_VACIA), 0, 0);
	hoja4_2->setIsActive(false);

	gameObjects.push_back(hoja1);
	gameObjects.push_back(hoja2);
	gameObjects.push_back(hoja3);
	gameObjects.push_back(hoja4);
	gameObjects.push_back(hoja4_2);
	
	overlays.push_back(hoja4_2);

	
	
#pragma endregion

#pragma region TEXTOS DEL MANUSCRITO
	// Cargamos el archivo de textos
	LoadTexts* textsLoader = new LoadTexts("../assets/data/texts.txt");
	// Recorremos las páginas y los textos para crearlos
 	for (int i = 0; i < pagesCount; i++) {

		if (i % 2 == 0) {
			manuscrito->changePage(i);
		}

		for (int j = 0; j < textsLoader->getTextsCount(i); j++) {

			// La data del texto (pos, tamaño, contenido)
			TextData textData = textsLoader->getTextData(i, j);

			//CREAMOS EL TEXTO
			GameObject* texto = new GameObject("Texto " + to_string(i) + "_" + to_string(j), 4, bookPages[i]);

			// Añadimos los componentes
			texto->addComponent<Transform>(Vector2D<float>(textData.position.x, textData.position.y), 0.15);
			texto->addComponent<SpriteRenderer>();
			texto->addComponent<Text>(textData.text, textData.color, manuscritoFont, textData.textEnd, textData.size, renderer);

			Button* btT = texto->addComponent<Button>();
			btT->onClick = [this, texto]() {
				showText(texto);
				};

			gameObjects.push_back(texto);
			texts.push_back(texto);
		}
	}

	hoja4_2->setChildren(hoja4->getChildren());
	hoja4->removeChildren();
	hoja4_2->setIsActive(false);

	delete textsLoader;
#pragma endregion

#pragma region RunicTest
	/*
	*  1. Creamos el GameObject
	*  2. Añadimos transform y renderer
	*  3. Añadimos el selector
	*  4. Añadimos el boton y le agregamos el metodo onClick de selector
	*  5. Ocultamos el render
	*  6. Se añade a los vectores y a runicTest
	*/
	
	GameObject* selector1 = new GameObject("Selector1", 4, hoja4);
	selector1->addComponent<Transform>(Vector2D<float>(9, -60), 0.06);
	selector1->addComponent<SpriteRenderer>(getTexture(SELECTOR), 0, 0);
	Selector* sl1 = selector1->addComponent<Selector>();
	Button* btS1 = selector1->addComponent<Button>();
	btS1->onClick = [sl1]() { sl1->onClick(); };

	GameObject* selector2 = new GameObject("Selector2", 4, hoja4);
	selector2->addComponent<Transform>(Vector2D<float>(37, -50), 0.06);
	selector2->addComponent<SpriteRenderer>(getTexture(SELECTOR), 0, 0);
	Selector* sl2 = selector2->addComponent<Selector>();
	Button* btS2 = selector2->addComponent<Button>();
	btS2->onClick = [sl2]() { sl2->onClick(); };

	GameObject* selector3 = new GameObject("Selector3", 4, hoja4);
	selector3->addComponent<Transform>(Vector2D<float>(57, -30), 0.06);
	selector3->addComponent<SpriteRenderer>(getTexture(SELECTOR), 0, 0);
	Selector* sl3 = selector3->addComponent<Selector>();
	Button* btS3 = selector3->addComponent<Button>();
	btS3->onClick = [sl3]() { sl3->onClick(); };

	GameObject* selector4 = new GameObject("Selector4", 4, hoja4);
	selector4->addComponent<Transform>(Vector2D<float>(60, -3), 0.06);
	selector4->addComponent<SpriteRenderer>(getTexture(SELECTOR), 0, 0);
	Selector* sl4 = selector4->addComponent<Selector>();
	Button* btS4 = selector4->addComponent<Button>();
	btS4->onClick = [sl4]() { sl4->onClick(); };

	GameObject* selector5 = new GameObject("Selector5", 4, hoja4);
	selector5->addComponent<Transform>(Vector2D<float>(53, 30), 0.06);
	selector5->addComponent<SpriteRenderer>(getTexture(SELECTOR), 0, 0);
	Selector* sl5 = selector5->addComponent<Selector>();
	Button* btS5 = selector5->addComponent<Button>();
	btS5->onClick = [sl5]() { sl5->onClick(); };

	GameObject* selector6 = new GameObject("Selector6", 4, hoja4);
	selector6->addComponent<Transform>(Vector2D<float>(37, 57), 0.06); 
	selector6->addComponent<SpriteRenderer>(getTexture(SELECTOR), 0, 0);
	Selector* sl6 = selector6->addComponent<Selector>();
	Button* btS6 = selector6->addComponent<Button>();
	btS6->onClick = [sl6]() { sl6->onClick(); };

	GameObject* selector7 = new GameObject("Selector7", 4, hoja4); 
	selector7->addComponent<Transform>(Vector2D<float>(2, 67), 0.06); 
	selector7->addComponent<SpriteRenderer>(getTexture(SELECTOR), 0, 0);
	Selector* sl7 = selector7->addComponent<Selector>();
	Button* btS7 = selector7->addComponent<Button>();
	btS7->onClick = [sl7]() { sl7->onClick(); };

	GameObject* selector8 = new GameObject("Selector8", 4, hoja4); 
	selector8->addComponent<Transform>(Vector2D<float>(-30, 60), 0.06);
	selector8->addComponent<SpriteRenderer>(getTexture(SELECTOR), 0, 0);
	Selector* sl8 = selector8->addComponent<Selector>();
	Button* btS8 = selector8->addComponent<Button>();
	btS8->onClick = [sl8]() { sl8->onClick(); };

	GameObject* selector9 = new GameObject("Selector9", 4, hoja4);
	selector9->addComponent<Transform>(Vector2D<float>(-50, 35), 0.06); 
	selector9->addComponent<SpriteRenderer>(getTexture(SELECTOR), 0, 0);
	Selector* sl9 = selector9->addComponent<Selector>();
	Button* btS9 = selector9->addComponent<Button>();
	btS9->onClick = [sl9]() { sl9->onClick(); };

	GameObject* selector10 = new GameObject("Selector10", 4, hoja4); 
	selector10->addComponent<Transform>(Vector2D<float>(-56, 0), 0.06); 
	selector10->addComponent<SpriteRenderer>(getTexture(SELECTOR), 0, 0);
	Selector* sl10 = selector10->addComponent<Selector>();
	Button* btS10 = selector10->addComponent<Button>();
	btS10->onClick = [sl10]() { sl10->onClick(); };

	GameObject* selector11 = new GameObject("Selector11", 4, hoja4); 
	selector11->addComponent<Transform>(Vector2D<float>(-50, -33), 0.06); 
	selector11->addComponent<SpriteRenderer>(getTexture(SELECTOR), 0, 0);
	Selector* sl11 = selector11->addComponent<Selector>();
	Button* btS11 = selector11->addComponent<Button>();
	btS11->onClick = [sl11]() { sl11->onClick(); };

	GameObject* selector12 = new GameObject("Selector12", 4, hoja4);
	selector12->addComponent<Transform>(Vector2D<float>(-25, -55), 0.06);
	selector12->addComponent<SpriteRenderer>(getTexture(SELECTOR), 0, 0);
	Selector* sl12 = selector12->addComponent<Selector>();
	Button* btS12 = selector12->addComponent<Button>();
	btS12->onClick = [sl12]() { sl12->onClick(); };

	overlays.push_back(selector1);
	overlays.push_back(selector2);
	overlays.push_back(selector3);
	overlays.push_back(selector4);
	overlays.push_back(selector5);
	overlays.push_back(selector6);
	overlays.push_back(selector7);
	overlays.push_back(selector8);
	overlays.push_back(selector9);
	overlays.push_back(selector10);
	overlays.push_back(selector11);
	overlays.push_back(selector12);
	gameObjects.push_back(selector1);
	gameObjects.push_back(selector2);
	gameObjects.push_back(selector3);
	gameObjects.push_back(selector4);
	gameObjects.push_back(selector5);
	gameObjects.push_back(selector6);
	gameObjects.push_back(selector7);
	gameObjects.push_back(selector8);
	gameObjects.push_back(selector9);
	gameObjects.push_back(selector10);
	gameObjects.push_back(selector11);
	gameObjects.push_back(selector12);

	runicTest = new RunicTest(RUNIC_TEST_SOLUTION, RUNIC_TEST_LENGHT, manuscrito, hoja4_2);
	runicTest->addSelector(sl1);
	runicTest->addSelector(sl2);
	runicTest->addSelector(sl3);
	runicTest->addSelector(sl4);
	runicTest->addSelector(sl5);
	runicTest->addSelector(sl6);
	runicTest->addSelector(sl7);
	runicTest->addSelector(sl8);
	runicTest->addSelector(sl9);
	runicTest->addSelector(sl10);
	runicTest->addSelector(sl11);
	runicTest->addSelector(sl12);

#pragma endregion

	manuscrito->changePage(0);
}

#pragma region ButtonEvents

// Muestra el texto asociado al botón
void Game::showText(GameObject* text) {
	if (glasses) {
		// Si ya hay un texto mostrado, lo ocultamos
		if (currentText != nullptr && currentText != text) {
			Text* textComp = currentText->getComponent<Text>();
			textComp->changeFont(manuscritoFont, textComp->getFontSize());
		}

		// Mostramos el texto seleccionado
		Text* t = text->getComponent<Text>();
		t->changeFont(baseFont, t->getFontSize() - 16);
		currentText = text;
	}
}
#pragma endregion

#pragma region Auxiliar

void Game::renderObjects() const{
	getTexture(BACKGROUND)->render();

	// El fondo del cuaderno
	for (size_t i = 0; i < pagesCount; i++) {
		if (bookPages[i]->getIsActive() && bookPages[i]->spriteRenderer != nullptr && bookPages[i]->spriteRenderer->isEnabled)
			bookPages[i]->render();
	}

	// EL texto
	for (size_t i = 0; i < texts.size(); i++) {
		if (texts[i]->getIsActive() && texts[i]->spriteRenderer != nullptr && texts[i]->spriteRenderer->isEnabled)
			texts[i]->render();
	}

	// Elementos extra
	for (size_t i = 0; i < overlays.size(); i++) {
		if (overlays[i]->getIsActive() && overlays[i]->spriteRenderer != nullptr && overlays[i]->spriteRenderer->isEnabled)
			overlays[i]->render();
	}
}

#pragma endregion
