#pragma once
#include "Scene.h"
class CerrarScene : public Scene
{
private:
	const char* const INTRO_TEXT_DIR = "../assets/data/finalCerrarTexts.txt";
	const float SHOW_TEXT_SPEED = 3.0f;
	const int BUTTON_HEIGHT = 400;
	const int BUTTON_SIZE = 100;

	std::vector<GameObject*> texts;
	GameObject* prevText = nullptr;
	GameObject* continueButton = nullptr;
	int textsCount = 0;
	int currentText = 0;

	bool canContinue;

public:
	CerrarScene(Game& game) : Scene(game) { }

	void Load() override;

private:
	void loadTexts(std::istream& file, std::string& btnText);
	void Render() override;
	void nextText();
	void fadeOutText();
	void onFadeInEnd();
};

