#pragma once
#include "Component.h"
#include "vector2D.h"
#include <SDL3/SDL.h>

// Sirve para saber si la escala es por defecto o si se multiplica el w y h de la textura por un escalar
enum ScaleMode {
	DEFAULT,
	SCALAR
};

class Transform : public Component
{
private:
	Vector2D<float> textureSize;
	Vector2D<float> position;
	Vector2D<float> scale;

	Transform* parentTransform = nullptr;

	ScaleMode scaleMode = ScaleMode::DEFAULT;

	Vector2D<float> getParentPosition() const;

public:
	// Rectángulo destino para renderizar con SDL
	SDL_FRect dstRect;

	Transform();
	Transform(Vector2D<float> position, Vector2D<float> size);
	Transform(Vector2D<float> position, float size);

	void setScaleMode(ScaleMode mode);

	Vector2D<float> getScale() const;
	Vector2D<float> getPosition() const;
	Vector2D<float> getGlobalPosition() const;
	void setPosition(Vector2D<float> newPosition);
	void setScale(Vector2D<float> neScale);

	void updateTextureSize(Vector2D<float> size);

	void onComponentAdd() override;
};

