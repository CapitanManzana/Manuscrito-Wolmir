#pragma once
#include "texture.h"
#include "Transform.h"

class SpriteRenderer : public Component
{
private: 
	Texture* texture = nullptr;
	Transform* transform = nullptr;

	int row;
	int col;

public:
	SpriteRenderer();
	SpriteRenderer(Texture* texture, int row, int col);
	~SpriteRenderer();

	void render();
};

