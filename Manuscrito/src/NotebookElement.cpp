#include "NotebookElement.h"
#include "Text.h"
#include "GameObject.h"

#pragma region Constructores y destructores
NotebookElement::NotebookElement(const std::string& title, int fSize) {
	this->title = title;
	this->fontSize = fSize;
}
#pragma endregion

void NotebookElement::reveal() {
	Text* targetText = gameObject->getComponent<Text>();
	if (!targetText) return;

	gameObject->spriteRenderer->isEnabled = true;
	GameObject* child = gameObject->getChild(0);

	if (child) {
		child->spriteRenderer->isEnabled = true;
	}

	if (!discovered) {
		targetText->setText("?", fontSize);
	}
	else {
		targetText->setText(title, fontSize);
	}
}