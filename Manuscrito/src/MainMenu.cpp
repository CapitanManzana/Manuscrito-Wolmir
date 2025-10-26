#include "MainMenu.h"
#include "GameObject.h"
#include "Book.h"
#include "texture.h"
#include "LoadTexts.h"
#include "Text.h"
#include "Button.h"
#include "Fader.h"
#include "Hover.h"
#include "SDL3/SDL_image.h"
#include "AudioManager.h"

using namespace std;

MainMenu::~MainMenu() {
	delete libro;
}

void MainMenu::Load() {
	GameObject* hoja1 = new GameObject("Hoja1", 2);
	hoja1->addComponent<Transform>(Vector2D<float>(1, 1), 0.1875);
	hoja1->addComponent<SpriteRenderer>(game->getTexture(Game::HOJA_VACIA), 0, 0);

	GameObject* hoja2 = new GameObject("Hoja2", 2);
	hoja2->addComponent<Transform>(Vector2D<float>(1, 1), 0.1875);
	hoja2->addComponent<SpriteRenderer>(game->getTexture(Game::HOJA_VACIA), 0, 0);

	GameObject* hoja3 = new GameObject("Hoja3", 2);
	hoja3->addComponent<Transform>(Vector2D<float>(1, 1), 0.1875);
	hoja3->addComponent<SpriteRenderer>(game->getTexture(Game::HOJA_VACIA), 0, 0);

	GameObject* hoja4 = new GameObject("Hoja4", 2);
	hoja4->addComponent<Transform>(Vector2D<float>(1, 1), 0.1875);
	hoja4->addComponent<SpriteRenderer>(game->getTexture(Game::HOJA_VACIA), 0, 0);

	pages.push_back(hoja1);
	pages.push_back(hoja2);
	pages.push_back(hoja3);
	pages.push_back(hoja4);

	libro = new Book(pages, Game::WINDOW_WIDTH / 2 - 5, Game::WINDOW_HEIGHT / 2 - 9, 125);
	pagesCount = libro->getPageCount();

	game->gameObjects.push_back(hoja1);
	game->gameObjects.push_back(hoja2);
	game->gameObjects.push_back(hoja3);
	game->gameObjects.push_back(hoja4);

	sceneObjects.push_back(hoja1);
	sceneObjects.push_back(hoja2);
	sceneObjects.push_back(hoja3);
	sceneObjects.push_back(hoja4);

	// Cargamos el archivo de textos
	LoadTexts* textsLoader = new LoadTexts(menuTextsData);
	// Recorremos las páginas y los textos para crearlos
	for (int i = 0; i < pagesCount; i++) {

		if (i % 2 == 0) {
			libro->changePage(i);
		}

		for (int j = 0; j < textsLoader->getTextsCount(i); j++) {

			// La data del texto (pos, tamaño, contenido)
			TextData textData = textsLoader->getTextData(i, j);

			//CREAMOS EL TEXTO
			GameObject* texto = new GameObject("Texto " + to_string(i) + "_" + to_string(j), 4, pages[i]);
			// Añadimos los componentes
			texto->addComponent<Transform>(Vector2D<float>(textData.position.x, textData.position.y), 0.15);
			texto->addComponent<SpriteRenderer>();
			Text* t = texto->addComponent<Text>(textData.text, textData.color, game->baseFontCentered, textData.textEnd, textData.size, renderer);

			if (i == 1 && j == 4) {
				Button* b = texto->addComponent<Button>();
				b->onClick = [this]() { changeScene(INTRO); };

				SDL_Color hoverColor = { 100, 0, 0, 255 };
				SDL_Color color = textData.color;

				Hover* h = texto->addComponent<Hover>(renderer);
				h->onEnterHover = [t, hoverColor]() { t->setColor(hoverColor); };
				h->onExitHover = [t, color]() { t->setColor(color); };
			}
			else if (i == 1 && j == 5) {
				Button* b = texto->addComponent<Button>();
				b->onClick = [this]() { exitGame(); };

				SDL_Color hoverColor = { 100, 0, 0, 255 };
				SDL_Color color = textData.color;

				Hover* h = texto->addComponent<Hover>(renderer);
				h->onEnterHover = [t, hoverColor]() { t->setColor(hoverColor); };
				h->onExitHover = [t, color]() { t->setColor(color); };
			}

			game->gameObjects.push_back(texto);
			sceneObjects.push_back(texto);
		}
	}

	libro->changePage(0);

	delete textsLoader;

	fader = new GameObject("Fader", 3);
	fader->addComponent<Transform>(Vector2D<float>(Game::WINDOW_WIDTH / 2, Game::WINDOW_HEIGHT / 2), 1);
	fader->addComponent<SpriteRenderer>(game->getTexture(Game::FADER), 0, 0);
	bool starFadeIn = false;
	fader->addComponent<Fader>(fadeTime, starFadeIn);

	sceneObjects.push_back(fader);
	game->gameObjects.push_back(fader);

	// LUZ AMBIENTE
	SDL_Surface* ambientLightSurf = IMG_Load("../assets/images/AmbientLight.png");

	rect_ambientLight = { lightPosition.x, lightPosition.y, static_cast<float>(ambientLightSurf->w) * lightRadius, static_cast<float>(ambientLightSurf->h) * lightRadius };
	ambientLight_tex = SDL_CreateTextureFromSurface(renderer, ambientLightSurf);
	SDL_SetTextureBlendMode(ambientLight_tex, SDL_BLENDMODE_ADD);

	rendertex_ambientLight = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT);
	SDL_SetTextureBlendMode(rendertex_ambientLight, SDL_BLENDMODE_MOD);

	rendertexLight = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT);
	SDL_SetTextureBlendMode(rendertexLight, SDL_BLENDMODE_MOD);

	SDL_DestroySurface(ambientLightSurf);
}

void MainMenu::Reload() {
	fader->getComponent<Fader>()->resetFade();
}

void MainMenu::Render() {
	// 1. PREPARAR LA MÁSCARA DE LUZ
	SDL_SetRenderDrawColor(renderer, ambientLightDarkZone.r, ambientLightDarkZone.g, ambientLightDarkZone.b, ambientLightDarkZone.a);
	SDL_SetRenderTarget(renderer, rendertex_ambientLight);
	SDL_RenderClear(renderer);
	SDL_RenderTexture(renderer, ambientLight_tex, NULL, &rect_ambientLight);
	SDL_SetRenderTarget(renderer, NULL);
	SDL_RenderClear(renderer);

	game->getTexture(Game::BACKGROUND)->render();
	for (GameObject* g : sceneObjects) {
		if (g->getIsActive() && g->spriteRenderer != nullptr && g->spriteRenderer->isEnabled) {
			g->render();
		}
	}

	SDL_SetRenderDrawColor(renderer, ambientLightIlumZone.r, ambientLightIlumZone.g, ambientLightIlumZone.b, ambientLightIlumZone.a);
	SDL_RenderFillRect(renderer, nullptr);

	SDL_RenderTexture(renderer, rendertex_ambientLight, NULL, NULL);
}

void MainMenu::HandleEvents(SDL_Event& event) {
	if (event.type == SDL_EVENT_KEY_DOWN) {

		// CAMBIO DE PÁGINA DERECHA
		if (event.key.key == SDLK_D) {
			currentPage += 2;
			if (currentPage >= pagesCount) currentPage = pagesCount - 2;

			libro->changePage(currentPage);
		}

		// CAMBIO DE PÁGINA IZQUIERDA
		if (event.key.key == SDLK_A) {
			currentPage -= 2;
			if (currentPage < 0) currentPage = 0;

			libro->changePage(currentPage);
		}
	}
}

void MainMenu::changeScene(SceneType scene) {
	Fader* f = fader->getComponent<Fader>();
	AudioManager::playSound(AudioManager::BUTTON);

	if (f && !f->onAnimation) {
		f->startFadeIn();
		f->onFadeInEnd = [scene]() { SceneManager::changeScene(scene); };
		AudioManager::playSong(AudioManager::THE_RIVER);
	}
}

void MainMenu::exitGame() {
	AudioManager::playSound(AudioManager::BUTTON);
	game->exitGame();
}