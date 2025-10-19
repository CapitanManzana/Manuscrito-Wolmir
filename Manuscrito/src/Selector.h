#pragma once
#include "Component.h"

class RunicTest;
class SpriteRenderer;

class Selector : public Component
{
private:
	RunicTest* test = nullptr;
	int index;

public:
	Selector() = default;
	~Selector();

	bool active = false;

	void deSelect();
	void onClick();
	void setData(int i, RunicTest* test);

	void onComponentAdd() override;
};

