#pragma once
#include "Component.h"
#include "vector2D.h"
#include <SDL3/SDL.h>

class Transform : public Component
{
private:
	Vector2D<float> textureSize;
	Vector2D<float> position;
	Vector2D<float> scale;
public:
	// Rectángulo destino para renderizar con SDL
	SDL_FRect dstRect;

	Transform();
	Transform(Vector2D<float> position, Vector2D<float> size);
	Transform(Vector2D<float> position, float size);

	bool overlapingPoint(Vector2D<float> point) const;

	Vector2D<float> getScale() const;
	Vector2D<float> getPosition() const;
	void setPosition(Vector2D<float> newPosition);
	void setScale(Vector2D<float> neScale);

	void updateTextureSize(Vector2D<float> size);

	void onComponentAdd() override;
};

