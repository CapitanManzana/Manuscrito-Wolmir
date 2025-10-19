#pragma once
#include <vector>
#include "MonoBehaviour.h"

class GameObject;
class Selector;
class Book;

typedef unsigned short mask;

class RunicTest : public MonoBehaviour
{
private:
	Book* book = nullptr;
	GameObject* newPage = nullptr;
	std::vector<Selector*> selectors;

	mask solutionCode = 1;
	mask currentCode = 0;

	int selectorsCount = 0;
	int codeLength = 1;
	int currentCodeLength = 0;

	void clearMask();
	bool checkCode();

	bool completed = false;

public:
	RunicTest() = default;
	RunicTest(mask solution, int length, Book* book, GameObject* newPage);

	void addSelector(Selector* sl);
	void updateState(int index);
	void onSuccess();

	void Update(float deltaTime) override;
};

