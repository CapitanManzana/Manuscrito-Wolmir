#pragma once
#include "Component.h"
#include "vector2D.h"
#include <SDL3/SDL.h>

class Transform : public Component
{
public:
	Vector2D<float> position;
	Vector2D<float> scale;

	// Rectángulo destino para renderizar con SDL
	SDL_FRect dstRect;

	Transform();
	Transform(Vector2D<float> position, Vector2D<float> size);

	bool overlapingPoint(Vector2D<float> point) const;

	Vector2D<float> getSize() const;
	Vector2D<float> getPosition() const;
	void setPosition(Vector2D<float> newPosition);

};

