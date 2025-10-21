#include "Selector.h"
#include "RunicTest.h"
#include "SpriteRenderer.h"
#include "GameObject.h"

Selector::~Selector() {
	test = nullptr;
}

void Selector::onClick() {
	if (test) {
		test->updateState(index);
		active = !active;

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
	//gameObject->spriteRenderer->isEnabled = false;
}