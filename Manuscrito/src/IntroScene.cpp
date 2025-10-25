#include "IntroScene.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "Text.h"
#include "Fader.h"
#include "Button.h"

#include <fstream>

using namespace std;

void IntroScene::Load() {
	fstream file(INTRO_TEXT_DIR);
	string btonText;

	if (file.is_open()) {
		loadTexts(file, btonText);
	}
	else {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error al abrir el archivo de textos de intro");
	}

	continueButton = new GameObject("ContinueButton", 5);
	continueButton->addComponent<Transform>(Vector2D<float>(Game::WINDOW_WIDTH / 2, BUTTON_HEIGHT), 0.15);
	continueButton->addComponent<SpriteRenderer>();
	SDL_Color color = { 196, 26, 0, 255 };
	continueButton->addComponent<Text>(btonText, color, game->baseFontCentered, 0, BUTTON_SIZE, renderer);
	continueButton->addComponent<Fader>(SHOW_TEXT_SPEED, true);
	continueButton->addComponent<Button>([this]() { fadeOutText();});

	game->gameObjects.push_back(continueButton);
	sceneObjects.push_back(continueButton);

	nextText();
}

void IntroScene::loadTexts(istream& file, string& buttonText) {
	string tag;
	file >> tag >> textsCount;
	file >> tag;
	getline(file, buttonText);

	for (int i = 0; i < textsCount; i++) {
		/*
		* T0:
			Text: Febrero de 1972
			PosX: -1 -> esta en el centro
			PosY: -1 -> esta en el centro
			Size: 100
			EndText: 4000
		*/
		file >> tag; // TX:

		string text;
		float posX, posY;
		int size, endText;

		file >> tag;
		getline(file, text);
		file >> tag >> posX;
		file >> tag >> posY;
		file >> tag >> size;
		file >> tag >> endText;

		GameObject* texto = new GameObject("Texto", 3);
		SDL_Color color = { 255, 255, 255, 255 };

		// Añadimos los componentes
		if (posX == -1) {
			posX = Game::WINDOW_WIDTH / 2;
		}
		if (posY == -1) {
			posY = Game::WINDOW_HEIGHT / 2;
		}

		texto->addComponent<Transform>(Vector2D<float>(posX, posY), 0.15);
		texto->addComponent<SpriteRenderer>();
		texto->addComponent<Text>(text, color, game->baseFontCentered, endText, size, renderer);
		bool starFadeIn = true;
		Fader* f = texto->addComponent<Fader>(SHOW_TEXT_SPEED, starFadeIn);
		f->onFadeOutEnd = [this]() { nextText(); };
		f->onFadeInEnd = [this]() { onFadeInEnd(); };

		texto->setIsActive(false);

		game->gameObjects.push_back(texto);
		sceneObjects.push_back(texto);
		texts.push_back(texto);
	}
}

void IntroScene::nextText() {
	if (currentText < textsCount) {
		texts[currentText]->setIsActive(true);
		prevText = texts[currentText];
		currentText++;
	}
	else {
		SceneManager::changeScene(1);
	}
}

void IntroScene::fadeOutText() {
	if (!canContinue) {
		return;
	}

	canContinue = false;
	if (currentText < textsCount) {
		Fader* f = prevText->getComponent<Fader>();
		if (f) {
			f->startFadeOut();
		}
	}
	else {
		Fader* f = prevText->getComponent<Fader>();
		Fader* bf = continueButton->getComponent<Fader>();

		if (f && bf) {
			f->startFadeOut();
			bf->startFadeOut();
		}
	}
}

void IntroScene::onFadeInEnd() {
	canContinue = true;
}