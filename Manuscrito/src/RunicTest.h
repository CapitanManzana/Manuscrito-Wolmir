#pragma once
#include <vector>
#include "MonoBehaviour.h"

class GameObject;
class Selector;
class Book;
class MainGame;

typedef unsigned short mask;

class RunicTest : public MonoBehaviour
{
private:
	MainGame* game;

	Book* book = nullptr;
	GameObject* newPage = nullptr;
	int pageIndex;
	std::vector<Selector*> selectors;

	mask solutionCode = 1;
	mask currentCode = 0;

	int selectorsCount = 0;
	int codeLength = 1;
	int currentCodeLength = 0;

	void clearMask();
	bool checkCode();

	bool completed = false;

	int id;
	static int testsCount;

public:
	RunicTest() = default;
	RunicTest(mask solution, int length, Book* book, GameObject* newPage, int pageIndex, MainGame* game);

	void addSelector(Selector* sl);
	void updateState(int index);
	void onSuccess();

	void Update(float deltaTime) override;
};

