#include "Book.h"
#include "GameObject.h"
#include "Transform.h"

Book::Book() {
	pagesNumber = 0;
	currentDisplayPage = 0;

	leftPage = nullptr;
	rightPage = nullptr;

	leftTransform = nullptr;
	rightTransform = nullptr;

	pageSeparation = 10;
	position = Vector2D<float>(0.0f, 0.0f);
}

Book::Book(const std::vector<GameObject*>& pages, float x, float y, float separation) {
	this->pages = pages;
	this->pagesNumber = this->pages.size();
	this->position = Vector2D<float>(x, y);
	this->pageSeparation = separation;

	// Muestra la primera página (o las dos primeras).
	changePage(0);
}

Book::~Book() {
	pagesNumber = 0;
	pages.clear();
	currentDisplayPage = 0;

	leftPage = nullptr;
	rightPage = nullptr;

	leftTransform = nullptr;
	rightTransform = nullptr;

	position = Vector2D<float>(0.0f, 0.0f);
}

void Book::changePage(size_t index) {
	if (index >= pagesNumber) {
		SDL_LogWarn(0, "[Book] Índice de página inválido: %zu", index);
		return;
	}

	// 1. Oculta todas las páginas primero.
	for (auto& page : pages) {
		if (page) page->setIsActive(false);
	}

	currentDisplayPage = index;

	// 2. Muestra y posiciona la página izquierda.
	leftPage = pages[index];
	leftPage->setIsActive(true);
	if (auto t = leftPage->getComponent<Transform>()) {
		t->setPosition(Vector2D<float>(position.x - pageSeparation, position.y));
	}

	// 3. Muestra y posiciona la página derecha (si existe).
	if (index + 1 < pagesNumber) {
		rightPage = pages[index + 1];
		rightPage->setIsActive(true);
		if (auto t = rightPage->getComponent<Transform>()) {
			t->setPosition(Vector2D<float>(position.x + pageSeparation, position.y));
		}
	}
	else {
		rightPage = nullptr;
	}
}

void Book::remplacePage(GameObject* page, int index) {
	GameObject* p = pages[index];

	if (p == leftPage) {
		p->setIsActive(false);
		pages[index] = page;
		leftPage = page;
		page->setIsActive(true);

		leftPage->setIsActive(true);
		if (auto t = leftPage->getComponent<Transform>()) {
			t->setPosition(Vector2D<float>(position.x - pageSeparation, position.y));
		}
	}
	else if (p == rightPage) {
		p->setIsActive(false);
		pages[index] = page;
		rightPage = page;
		page->setIsActive(true);

		rightPage->setIsActive(true);
		if (auto t = rightPage->getComponent<Transform>()) {
			t->setPosition(Vector2D<float>(position.x + pageSeparation, position.y));
		}
	}
	else {
		p->setIsActive(false);
		pages[index] = page;
	}
}