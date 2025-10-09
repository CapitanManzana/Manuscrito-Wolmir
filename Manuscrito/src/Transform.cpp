#include "Transform.h"

Transform::Transform(Vector2D<float> position, Vector2D<float> size) {
	this->position = position;
	scale = size;

	dstRect = { position.x, position.y, size.x, size.y };
}

Vector2D<float> Transform::getSize() const {
	return scale;
}

Vector2D<float> Transform::getPosition() const {
	return position;
}

void Transform::setPosition(Vector2D<float> newPosition) {
	position = newPosition;
	dstRect.x = position.x;
	dstRect.y = position.y;
}