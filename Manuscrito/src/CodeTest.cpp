#include "CodeTest.h"
#include "Text.h"
#include "GameObject.h"

CodeTest::CodeTest(std::vector<Text*> texts, std::string solution)
{
	codeTexts = texts;
	correctSolution = solution;
	codeLength = static_cast<int>(texts.size());
}

void CodeTest::setInputCode(char c)
{
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
		SDL_Log("CodeTest: Correct code entered!");
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
