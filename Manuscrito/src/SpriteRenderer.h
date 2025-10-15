#pragma once
#include "Component.h"

class Texture;
class Transform;

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

	Texture* getTexture() const;
	void setTexture(Texture* texture);

	void onComponentAdd() override;

	void render();
};