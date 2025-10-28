#include "RunicTest.h"

#include "GameObject.h"
#include "Selector.h"
#include "MainGame.h"
#include "Book.h"
#include <bit>

#include <SDL3/SDL.h>

int RunicTest::testsCount = 0;
/// <summary>
/// Crea una nueva prueba r�nica, de seleccion de caracteres
/// </summary>
/// <param name="sol">mascara binaria de la solucion</param>
/// <param name="length">longitud de la solucion</param>
/// <param name="book">libro donde est� el test</param>
/// <param name="page">pagina a mostrar</param>
/// <param name="pageIndex">index de la p�gina</param>
RunicTest::RunicTest(mask sol, int length, Book* book, GameObject* page, int pageIndex, MainGame* game) {
	codeLength = length;
	solutionCode = sol;
	this->book = book;
	newPage = page;
	this->pageIndex = pageIndex;

	id = testsCount++;
	this->game = game;
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
	if (id == 0) game->discoveredBlackLight = true;
	AudioManager::playSong(AudioManager::DISCOVERY);
	if (book) {
		for (Selector* s : selectors) {
			s->gameObject->setIsActive(false);
		}

		book->remplacePage(newPage, pageIndex);
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