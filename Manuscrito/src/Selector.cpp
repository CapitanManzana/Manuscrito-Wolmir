#include "Selector.h"
#include "RunicTest.h"
#include "SpriteRenderer.h"
#include "GameObject.h"
#include "AudioManager.h"

Selector::~Selector() {
	test = nullptr;
}

void Selector::onClick() {
	if (test) {
		test->updateState(index);
		active = !active;

		AudioManager::playSound(AudioManager::PENCIL_CIRCLE);

		if (gameObject->spriteRenderer) {
			gameObject->spriteRenderer->isEnabled = active;
		}
	}
}

void Selector::setData(int i, RunicTest* test) {
	index = i;
	this->test = test;
}

void Selector::deSelect() {
	active = false;
	if (gameObject->spriteRenderer) {
		gameObject->spriteRenderer->isEnabled = false;
	}
}

void Selector::onComponentAdd() {
	gameObject->spriteRenderer->isEnabled = false;
}