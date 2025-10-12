#include "Transform.h"
#include "GameObject.h"

Transform::Transform() {
	position = Vector2D<float>(0.0f, 0.0f);
	scale = Vector2D<float>(1.0f, 1.0f);

	dstRect = { position.x, position.y, scale.x, scale.y };
}

Transform::Transform(Vector2D<float> position, Vector2D<float> size) {
	this->position = position;
	scale = size;

	dstRect = { position.x, position.y, size.x, size.y };
}

bool Transform::overlapingPoint(Vector2D<float> point) const {
	return (point.x >= position.x && point.x <= position.x + scale.x * textureSize.x &&
		point.y >= position.y && point.y <= position.y + scale.y * textureSize.y);
}

Vector2D<float> Transform::getScale() const {
	return scale;
}

Vector2D<float> Transform::getPosition() const {
	return position;
}

void Transform::setPosition(Vector2D<float> newPosition) {
	position = newPosition;
	dstRect.x = position.x - dstRect.w / 2;
	dstRect.y = position.y - dstRect.h / 2;
}

void Transform::setScale(Vector2D<float> newScale) {
	scale = newScale;
	dstRect.w = newScale.x;
	dstRect.h = newScale.y;
}

void Transform::updateTextureSize(Vector2D<float> size) {
	textureSize = size;
	dstRect.w = textureSize.x * scale.x;
	dstRect.h = textureSize.y * scale.y;
	dstRect.x = position.x - dstRect.w / 2;
	dstRect.y = position.y - dstRect.h / 2;
}

void Transform::onComponentAdd() {
	gameObject->transform = this;
}