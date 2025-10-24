#include "game.h"

#include <SDL3_image/SDL_image.h>
#include <fstream>

#include "texture.h"
#include "GameObject.h"
#include "Button.h"
#include "Text.h"
#include "Book.h"
#include "LoadTexts.h"
#include "Selector.h"
#include "RunicTest.h"
#include "Notebook.h"
#include "NoteRevealer.h"
#include "Hover.h"
#include "CodeTest.h"

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
constexpr const char* const notesData = "../assets/data/notes.txt";
constexpr const char* const textsData = "../assets/data/texts.txt";

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

#pragma region VARIABLES
// FUENTES
TTF_Font* Game::baseFont = nullptr;
TTF_Font* Game::baseFontCentered = nullptr;
TTF_Font* Game::manuscritoFont = nullptr;

// OBJETOS DEL JUEGO
vector<GameObject*> Game::gameObjects;

// MANUSCRITO
Book* manuscrito;
int currentPage = 0;
int pagesCount = 0;

//Cuaderno de notas
GameObject* notebookParent;
Notebook* notebook;

// TEXTOS
vector<GameObject*> Game::texts;
GameObject* currentText = nullptr;

// OVERLAY
vector<GameObject*> overlays;

// TESTS
RunicTest* runicTest;
RunicTest* astrologyTest;
CodeTest* finalCodeTest;

// HERAMIENTAS
bool blackLight = false;
bool glasses = false;
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

	//Carga los GameObjects
	createGameObjects();

	//Crea la luz uv
	createLight();
}

Game::~Game()
{
	TTF_CloseFont(baseFont);
	baseFont = nullptr;

	texts.clear();

	delete runicTest;
	delete astrologyTest;
	delete finalCodeTest;

	// Elimina los objetos del juego
	for (size_t i = 0; i < gameObjects.size(); i++) {
		delete gameObjects[i];
	}

	// Elimina las texturas
	for (size_t i = 0; i < textures.size(); i++) {
		delete textures[i];
	}

	delete manuscrito;
	delete notebook;

	SDL_DestroyTexture(light_tex);
	SDL_DestroyTexture(rendertex_light);
	SDL_DestroyTexture(rendertex);

	SDL_DestroyTexture(ambientLight_tex);
	SDL_DestroyTexture(rendertex_ambientLight);
	SDL_DestroyTexture(rendertexLight);


	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_Quit();
	SDL_Quit();
}

void
Game::render() const
{
	SDL_RenderClear(renderer);
	 
	// 1. PREPARAR LA MÁSCARA DE LUZ
	SDL_SetRenderDrawColor(renderer, ambientLightDarkZone.r, ambientLightDarkZone.g, ambientLightDarkZone.b, ambientLightDarkZone.a);
	// Apunta a tu textura de luz (que tiene BLENDMODE_MOD)
	SDL_SetRenderTarget(renderer, rendertex_ambientLight);
	// Limpia la textura de luz a negro (oscuridad total)
	SDL_RenderClear(renderer);
	// Renderiza el gradiente (light_tex, que tiene BLENDMODE_ADD) 
	// sobre la textura negra.
	SDL_RenderTexture(renderer, ambientLight_tex, NULL, &rect_ambientLight);

	// 2. RENDERIZAR LA ESCENA FINAL A LA PANTALLA

	// Apunta de nuevo a la pantalla
	SDL_SetRenderTarget(renderer, NULL);
	SDL_RenderClear(renderer); // Limpia la pantalla

	if (!blackLight) {
		renderObjects();

		if (glasses) {
			SDL_SetRenderDrawColor(renderer, 0, 115, 0, 1);
			SDL_RenderFillRect(renderer, nullptr);
		}
		else {
			SDL_SetRenderDrawColor(renderer, ambientLightIlumZone.r, ambientLightIlumZone.g, ambientLightIlumZone.b, ambientLightIlumZone.a);
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

		// Renderiza los elementos que se ven con la uv light si no esta activo el cuaderno
		if (!notebookParent->getIsActive()) {
			for (size_t i = 0; i < uvObjects.size(); i++) {
				if (uvObjects[i]->getIsActive() && uvObjects[i]->spriteRenderer != nullptr && uvObjects[i]->spriteRenderer->isEnabled)
					uvObjects[i]->render();
			}
		}
		// Multiplica el mapa por tu máscara de luz (rendertex_light)
		SDL_RenderTexture(renderer, rendertex_light, NULL, NULL);
	}

	// Multiplica el mapa por tu máscara de luz (rendertex_light)
	SDL_RenderTexture(renderer, rendertex_ambientLight, NULL, NULL);
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
	astrologyTest->DoUpdate(deltaTime);
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

		if (event.type == SDL_EVENT_TEXT_INPUT) {
			char c = event.text.text[0];

			if (c >= '0' && c <= '9') {
				SDL_Log("Input code: %c", c);
				finalCodeTest->setInputCode(c);
			}
		}

		if (event.type == SDL_EVENT_KEY_DOWN) {

			// LUZ UV
			if (event.key.key == SDLK_E) {
				blackLight = !blackLight;
			}

			// GAFAS DESCIFRADORAS
			if (event.key.key == SDLK_Q) {
				glasses = !glasses;
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

			if (event.key.key >= SDLK_0 && event.key.key <= SDLK_9) {
				if (currentPage == 6)
				{
					int number = event.key.key - SDLK_0;
					char c = '0' + number;

					finalCodeTest->setInputCode(c);
				}
			}

			if (event.key.key == SDLK_TAB) {
				notebookParent->setIsActive(!notebookParent->getIsActive());
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
	vector<GameObject*> bookPages;

	GameObject* hoja1 = new GameObject("Hoja1", 2);
	hoja1->addComponent<Transform>(Vector2D<float>(1, 1), 0.1875);
	hoja1->addComponent<SpriteRenderer>(getTexture(HOJA1), 0, 0);

	GameObject* hoja2 = new GameObject("Hoja2", 2);
	hoja2->addComponent<Transform>(Vector2D<float>(1, 1), 0.1875);
	hoja2->addComponent<SpriteRenderer>(getTexture(HOJA_VACIA), 0, 0);

	GameObject* hoja3 = new GameObject("Hoja3", 2);
	hoja3->addComponent<Transform>(Vector2D<float>(1, 1), 0.1875);
	hoja3->addComponent<SpriteRenderer>(getTexture(TABLA_RUNAS), 0, 0);

	GameObject* hoja4 = new GameObject("Hoja4", 2);
	hoja4->addComponent<Transform>(Vector2D<float>(1, 1), 0.1875);
	hoja4->addComponent<SpriteRenderer>(getTexture(RUNAS), 0, 0);

	GameObject* hoja5 = new GameObject("Hoja5", 2);
	hoja5->addComponent<Transform>(Vector2D<float>(1, 1), 0.1875);
	hoja5->addComponent<SpriteRenderer>(getTexture(HOJA5), 0, 0);

	GameObject* hoja6 = new GameObject("Hoja6", 2);
	hoja6->addComponent<Transform>(Vector2D<float>(1, 1), 0.1875);
	hoja6->addComponent<SpriteRenderer>(getTexture(HOJA6), 0, 0);

	GameObject* hoja7 = new GameObject("Hoja7", 2);
	hoja7->addComponent<Transform>(Vector2D<float>(1, 1), 0.1875);
	hoja7->addComponent<SpriteRenderer>(getTexture(HOJA_VACIA), 0, 0);

	GameObject* hoja8 = new GameObject("Hoja8", 2);
	hoja8->addComponent<Transform>(Vector2D<float>(1, 1), 0.1875);
	hoja8->addComponent<SpriteRenderer>(getTexture(HOJA8), 0, 0);

	bookPages.push_back(hoja1);
	bookPages.push_back(hoja2);
	bookPages.push_back(hoja3);
	bookPages.push_back(hoja4);
	bookPages.push_back(hoja5);
	bookPages.push_back(hoja6);
	bookPages.push_back(hoja7);
	bookPages.push_back(hoja8);

	manuscrito = new Book(bookPages, WINDOW_WIDTH / 2 - 5, WINDOW_HEIGHT / 2 - 9, 125);
	pagesCount = manuscrito->getPageCount();

	GameObject* hoja4_2 = new GameObject("Hoja4_2", 2);
	hoja4_2->addComponent<Transform>(Vector2D<float>(1, 1), 0.16);
	hoja4_2->addComponent<SpriteRenderer>(getTexture(HOJA_VACIA), 0, 0);
	hoja4_2->setIsActive(false);

	GameObject* hoja6_2 = new GameObject("Hoja6_2", 2);
	hoja6_2->addComponent<Transform>(Vector2D<float>(1, 1), 0.16);
	hoja6_2->addComponent<SpriteRenderer>(getTexture(HOJA_VACIA), 0, 0);
	hoja6_2->setIsActive(true);

	gameObjects.push_back(hoja1);
	gameObjects.push_back(hoja2);
	gameObjects.push_back(hoja3);
	gameObjects.push_back(hoja4);
	gameObjects.push_back(hoja4_2);
	gameObjects.push_back(hoja5);
	gameObjects.push_back(hoja6);
	gameObjects.push_back(hoja6_2);
	gameObjects.push_back(hoja7);
	gameObjects.push_back(hoja8);

#pragma endregion

#pragma region CUADERNO DE NOTAS
	notebookParent = new GameObject("Cuaderno", 2);
	notebookParent->addComponent<Transform>(Vector2D<float>(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2), 0.37);
	notebookParent->addComponent<SpriteRenderer>(getTexture(FOLIO), 0, 0);

	ifstream notesFile(notesData);

	if (notesFile.is_open()) {
		notebook = new Notebook(notesFile, notebookParent, baseFontCentered, getTexture(MARCO), renderer, this);
	}
	else {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't load notes data file");
	}

	// Cuaderno de notas
	gameObjects.push_back(notebookParent);
	notebookParent->setIsActive(false);

#pragma endregion

#pragma region TEXTOS DEL MANUSCRITO
	// Cargamos el archivo de textos
	LoadTexts* textsLoader = new LoadTexts(textsData);
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

			if (textData.revealNoteIndex != -1) {
				texto->addComponent<NoteRevealer>(textData.revealNoteIndex, notebook);
			}

			if (textData.uv) {
				uvObjects.push_back(texto);
			}
			else {
				texts.push_back(texto);
			}
			gameObjects.push_back(texto);
		}
	}
	//LOS HIJOS DE LA HOJA 4 PASAN A LA 4_2 PORQUE ESTAN OCULTOS
	//LOS HIJOS DE LA HOJA 6 SOLO NECESITAMOS LOS DEL FINAL DESPUES DEL 5 PORQUE SON LOS OCULTOS
	hoja4_2->setChildren(hoja4->getChildren());
	hoja4->removeChildren();

	//Recorremos los hijos y los metemos en un vector los necesarios
	vector<GameObject*> hoja6Children;
	for (int k = 5; k < hoja6->getChildren().size(); k++) {
		hoja6Children.push_back(hoja6->getChild(k));
	}
	hoja6_2->setChildren(hoja6Children);

	//Borramos los hijos
	int childsCount = hoja6->getChildren().size();
	for (int k = 5; k < childsCount; k++) {
		hoja6->removeChildren(5);
	}

	hoja4_2->setIsActive(false);
	hoja6_2->setIsActive(false);

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

	runicTest = new RunicTest(RUNIC_TEST_SOLUTION, RUNIC_TEST_LENGHT, manuscrito, hoja4_2, 3);
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

#pragma region Mapa Estelar
	GameObject* selector1E = new GameObject("Selector1E", 4, hoja5);
	selector1E->addComponent<Transform>(Vector2D<float>(71, -51), 0.03);
	selector1E->addComponent<SpriteRenderer>(getTexture(SELECTOR), 0, 0);
	Selector* sl1E = selector1E->addComponent<Selector>();
	Button* btS1E = selector1E->addComponent<Button>();
	btS1E->onClick = [sl1E]() { sl1E->onClick(); };

	GameObject* selector2E = new GameObject("Selector2E", 4, hoja5);
	selector2E->addComponent<Transform>(Vector2D<float>(-30, -55), 0.03);
	selector2E->addComponent<SpriteRenderer>(getTexture(SELECTOR), 0, 0);
	Selector* sl2E = selector2E->addComponent<Selector>();
	Button* btS2E = selector2E->addComponent<Button>();
	btS2E->onClick = [sl2E]() { sl2E->onClick(); };

	GameObject* selector3E = new GameObject("Selector3E", 4, hoja5);
	selector3E->addComponent<Transform>(Vector2D<float>(-107, 10), 0.03);
	selector3E->addComponent<SpriteRenderer>(getTexture(SELECTOR), 0, 0);
	Selector* sl3E = selector3E->addComponent<Selector>();
	Button* btS3E = selector3E->addComponent<Button>();
	btS3E->onClick = [sl3E]() { sl3E->onClick(); };

	GameObject* selector4E = new GameObject("Selector4E", 4, hoja5);
	selector4E->addComponent<Transform>(Vector2D<float>(-45, 77), 0.03);
	selector4E->addComponent<SpriteRenderer>(getTexture(SELECTOR), 0, 0);
	Selector* sl4E = selector4E->addComponent<Selector>();
	Button* btS4E = selector4E->addComponent<Button>();
	btS4E->onClick = [sl4E]() { sl4E->onClick(); };

	GameObject* selector5E = new GameObject("Selector5E", 4, hoja6);
	selector5E->addComponent<Transform>(Vector2D<float>(43, -53), 0.035);
	selector5E->addComponent<SpriteRenderer>(getTexture(SELECTOR), 0, 0);
	Selector* sl5E = selector5E->addComponent<Selector>();
	Button* btS5E = selector5E->addComponent<Button>();
	btS5E->onClick = [sl5E]() { sl5E->onClick(); };

	GameObject* selector6E = new GameObject("Selector6E", 4, hoja6);
	selector6E->addComponent<Transform>(Vector2D<float>(-73, 44), 0.03);
	selector6E->addComponent<SpriteRenderer>(getTexture(SELECTOR), 0, 0);
	Selector* sl6E = selector6E->addComponent<Selector>();
	Button* btS6E = selector6E->addComponent<Button>();
	btS6E->onClick = [sl6E]() { sl6E->onClick(); };

	GameObject* selector7E = new GameObject("Selector7E", 4, hoja6);
	selector7E->addComponent<Transform>(Vector2D<float>(23, 152), 0.03);
	selector7E->addComponent<SpriteRenderer>(getTexture(SELECTOR), 0, 0);
	Selector* sl7E = selector7E->addComponent<Selector>();
	Button* btS7E = selector7E->addComponent<Button>();
	btS7E->onClick = [sl7E]() { sl7E->onClick(); };

	overlays.push_back(selector1E);
	overlays.push_back(selector2E);
	overlays.push_back(selector3E);
	overlays.push_back(selector4E);
	overlays.push_back(selector5E);
	overlays.push_back(selector6E);
	overlays.push_back(selector7E);
	gameObjects.push_back(selector1E);
	gameObjects.push_back(selector2E);
	gameObjects.push_back(selector3E);
	gameObjects.push_back(selector4E);
	gameObjects.push_back(selector5E);
	gameObjects.push_back(selector6E);
	gameObjects.push_back(selector7E);

	astrologyTest = new RunicTest(ASTROLOGY_TEST_SOLUTION, ASTROLOGY_TEST_LENGHT, manuscrito, hoja6_2, 5);
	astrologyTest->addSelector(sl1E);
	astrologyTest->addSelector(sl2E);
	astrologyTest->addSelector(sl3E);
	astrologyTest->addSelector(sl4E);
	astrologyTest->addSelector(sl5E);
	astrologyTest->addSelector(sl6E);
	astrologyTest->addSelector(sl7E);

#pragma endregion

#pragma region Prueba fina

	manuscrito->changePage(6);
	vector<Text*> codeTexts;

	GameObject* code1 = new GameObject("code1", 3, hoja8);
	code1->addComponent<Transform>(Vector2D<float>(-75, 0), 0.15);
	code1->addComponent<SpriteRenderer>();
	SDL_Color color = { 128, 18, 0, 255 };
	Text* t1 = code1->addComponent<Text>("0", color, manuscritoFont, 0, 400, renderer);

	GameObject* code2 = new GameObject("code2", 3, hoja8);
	code2->addComponent<Transform>(Vector2D<float>(-25, 0), 0.15);
	code2->addComponent<SpriteRenderer>();
	Text* t2 = code2->addComponent<Text>("0", color, manuscritoFont, 0, 400, renderer);

	GameObject* code3 = new GameObject("code3", 3, hoja8);
	code3->addComponent<Transform>(Vector2D<float>(25, 0), 0.15);
	code3->addComponent<SpriteRenderer>();
	Text* t3= code3->addComponent<Text>("0", color, manuscritoFont, 0, 400, renderer);

	GameObject* code4 = new GameObject("code4", 3, hoja8);
	code4->addComponent<Transform>(Vector2D<float>(75, 0), 0.15);
	code4->addComponent<SpriteRenderer>();
	Text* t4 = code4->addComponent<Text>("0", color, manuscritoFont, 0, 400, renderer);

	codeTexts.push_back(t1);
	codeTexts.push_back(t2);
	codeTexts.push_back(t3);
	codeTexts.push_back(t4);

	finalCodeTest = new CodeTest(codeTexts, FINAL_CODE_SOLUTION);

	gameObjects.push_back(code1);
	gameObjects.push_back(code2);
	gameObjects.push_back(code3);
	gameObjects.push_back(code4);

#pragma endregion

	manuscrito->changePage(6);
}

#pragma region ButtonEvents

// Muestra el texto asociado al botón
void Game::showText(GameObject* text) {
	Text* t = text->getComponent<Text>();
	NoteRevealer* nr = text->getComponent<NoteRevealer>();
	if (glasses && !t->showText && !Text::showingText) {
		// Mostramos el texto seleccionado
		t->showText = true;
	}
}
#pragma endregion

#pragma region Auxiliar

void Game::renderObjects() const {
	getTexture(BACKGROUND)->render();

	// Renderizamos todo del cuaderno de notas
	if (notebookParent->getIsActive()) {
		notebook->render();
	}
	else 
	{
		// El fondo del cuaderno
		for (size_t i = 0; i < pagesCount; i++) {
			GameObject* page = manuscrito->getPage(i);
			if (page->getIsActive() && page->spriteRenderer != nullptr && page->spriteRenderer->isEnabled && page->getName() != "Hoja6_2")
				page->render();
		}

		// EL texto
		for (size_t i = 0; i < texts.size(); i++) {
			if (texts[i]->getIsActive() && texts[i]->spriteRenderer != nullptr && texts[i]->spriteRenderer->isEnabled)
				texts[i]->render();
		}

		//Los textos del final test
		finalCodeTest->render();

		// Elementos extra
		for (size_t i = 0; i < overlays.size(); i++) {
			if (overlays[i]->getIsActive() && overlays[i]->spriteRenderer != nullptr && overlays[i]->spriteRenderer->isEnabled)
				overlays[i]->render();
		}
	}
}

void Game::createLight() {

	// LUZ UV
	SDL_Surface* lightSurf = IMG_Load("../assets/images/light.png");
	SDL_Surface* ambientLightSurf = IMG_Load("../assets/images/AmbientLight.png");
	rectLight = { 0, 0, static_cast<float>(lightSurf->w), static_cast<float>(lightSurf->h) };
	light_tex = SDL_CreateTextureFromSurface(renderer, lightSurf);
	SDL_SetTextureBlendMode(light_tex, SDL_BLENDMODE_ADD);

	rendertex_light = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, WINDOW_WIDTH, WINDOW_HEIGHT);
	SDL_SetTextureBlendMode(rendertex_light, SDL_BLENDMODE_MOD);

	rendertex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, WINDOW_WIDTH, WINDOW_HEIGHT);
	SDL_SetTextureBlendMode(rendertex, SDL_BLENDMODE_MOD);

	// LUZ AMBIENTE
	rect_ambientLight = { lightPosition.x, lightPosition.y, static_cast<float>(lightSurf->w) * lightRadius, static_cast<float>(lightSurf->h) * lightRadius };
	ambientLight_tex = SDL_CreateTextureFromSurface(renderer, ambientLightSurf);
	SDL_SetTextureBlendMode(ambientLight_tex, SDL_BLENDMODE_ADD);

	rendertex_ambientLight = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, WINDOW_WIDTH, WINDOW_HEIGHT);
	SDL_SetTextureBlendMode(rendertex_ambientLight, SDL_BLENDMODE_MOD);

	rendertexLight = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, WINDOW_WIDTH, WINDOW_HEIGHT);
	SDL_SetTextureBlendMode(rendertexLight, SDL_BLENDMODE_MOD);

	SDL_DestroySurface(lightSurf);
	SDL_DestroySurface(ambientLightSurf);
}

#pragma endregion
