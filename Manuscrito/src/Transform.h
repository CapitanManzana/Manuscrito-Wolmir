#pragma once
#include "Component.h"
#include "vector2D.h"

class Transform : public Component
{
public:
	Vector2D<float> position;
	Vector2D<float> scale;

	// Rectángulo destino para renderizar con SDL
	SDL_FRect dstRect;

	Transform(Vector2D<float> position, Vector2D<float> size);
	Vector2D<float> getSize() const;
	Vector2D<float> getPosition() const;
	void setPosition(Vector2D<float> newPosition);
};

