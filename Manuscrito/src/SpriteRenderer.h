#pragma once
#include "Component.h"

class Texture;

class SpriteRenderer : public Component
{
private:
	Texture* texture = nullptr;

	int row;
	int col;

public:
	SpriteRenderer();
	SpriteRenderer(Texture* texture, int row, int col);
	~SpriteRenderer();

	void render();
};
