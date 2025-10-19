#pragma once
#include <vector>
#include "vector2D.h"

class GameObject;
class Transform;

class Book
{
private:
	size_t pagesNumber;
	std::vector<GameObject*> pages;

	int currentDisplayPage;

	GameObject* leftPage;
	GameObject* rightPage;

	Transform* leftTransform;
	Transform* rightTransform;

	Vector2D<float> position;
	int pageSeparation;
public:
	Book();
	Book(const std::vector<GameObject*> &pages, float x, float y, float separation);
	~Book();

	void changePage(size_t index);

	void remplacePage(GameObject* page, int index);

	GameObject* getPage(size_t index) const;
	size_t getPageCount() const;
	int getCurrentPage() const;
};

inline GameObject* Book::getPage(size_t index) const {
	if (index < pages.size()) {
		return pages[index];
	}
	return nullptr;
}

inline size_t Book::getPageCount() const {
	return pages.size();
}

inline int Book::getCurrentPage() const {
	return currentDisplayPage;
}
