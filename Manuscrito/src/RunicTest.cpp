#include "RunicTest.h"

#include "GameObject.h"
#include "Selector.h"
#include "Book.h"
#include <bit>

#include <SDL3/SDL.h>

RunicTest::RunicTest(mask sol, int length, Book* book, GameObject* page) {
	codeLength = length;
	solutionCode = sol;
	this->book = book;
	newPage = page;
}

void RunicTest::clearMask() {
	currentCode = 0;
	currentCodeLength = 0;
	for (Selector* sel : selectors) {
		sel->deSelect();
	}
}

// Hace un xnor para ver si es 0 donde es 0 y si es 1 donde es 1
bool RunicTest::checkCode() {
	return solutionCode == currentCode;
}

void RunicTest::addSelector(Selector* sl) {
	sl->setData(selectorsCount, this);
	selectors.push_back(sl);
	selectorsCount++;
}

void RunicTest::updateState(int index) {
	// Hacdemos xor para alternar -> 0 xor 1 = 1  1 xor 1 = 0
	mask mask = 1 << index;
	currentCode ^= mask;
	currentCodeLength = std::popcount(currentCode);
}

void RunicTest::onSuccess() {
	if (book) {
		for (Selector* s : selectors) {
			s->gameObject->setIsActive(false);
		}

		book->remplacePage(newPage, 3);
		completed = true;
	}
	else {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "No se ha encontrado puntero al libro en RunicTest");
	}
}

void RunicTest::Update(float deltaTime) {
	if (!completed) {
		if (currentCodeLength > codeLength) {
			clearMask();
		}

		if (checkCode()) {
			onSuccess();
		}
	}
}