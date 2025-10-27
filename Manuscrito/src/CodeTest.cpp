#include "CodeTest.h"
#include "Text.h"
#include "GameObject.h"
#include "Book.h"
#include "AudioManager.h"

CodeTest::CodeTest(std::vector<Text*> texts, std::string solution, Book& book, GameObject* page, int pageIndex)
{
	codeTexts = texts;
	correctSolution = solution;
	codeLength = static_cast<int>(texts.size());

	CodeTest::book = &book;
	CodeTest::page = page;
	CodeTest::pageIndex = pageIndex;
}


void CodeTest::setInputCode(char c)
{
	AudioManager::playSound(AudioManager::BUTTON);

	if (selectedTextIndex >= codeLength)
	{
		selectedTextIndex = 0;
		currentText = "";
		for (auto& text : codeTexts)
		{
			text->setText("0");
		}
	}

	std::string s(1, c);
	codeTexts[selectedTextIndex]->setText(s);

	// Actualizar el texto actual y el índice seleccionado
	currentText += s;
	selectedTextIndex++;

	if (currentText == correctSolution)
	{
		for (Text* t : codeTexts)
		{
			GameObject* g = t->gameObject;
			g->setIsActive(false);

			book->remplacePage(page, pageIndex);
			AudioManager::playSong(AudioManager::FINAL_CERRAR);
		}
	}
}

void CodeTest::render() const {
	for (Text* t : codeTexts)
	{
		GameObject* g = t->gameObject;
		if (g->getIsActive() && g->spriteRenderer != nullptr && g->spriteRenderer->isEnabled)
			g->render();
	}
}
