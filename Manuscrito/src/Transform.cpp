#include "Transform.h"
#include "GameObject.h"

#pragma region Constructores

Transform::Transform() {
	position = Vector2D<float>(0.0f, 0.0f);
	scale = Vector2D<float>(1.0f, 1.0f);

	dstRect = { position.x, position.y, scale.x, scale.y };
}

Transform::Transform(Vector2D<float> position, Vector2D<float> size) {
	this->position = position;
	scale = size;

	dstRect = { position.x, position.y, scale.x, scale.y };

}

Transform::Transform(Vector2D<float> position, float size) {
	this->position = position;
	scale = {
		size,
		size
	};

	dstRect = { position.x, position.y, scale.x, scale.y };
}

#pragma endregion

// Revisa si un punto está dentro del área del Transform
bool Transform::overlapingPoint(Vector2D<float> point) const {
	Vector2D position;

	if (gameObject->parent != nullptr && gameObject->parent->transform != nullptr) {
		float parentX = gameObject->parent->transform->position.x;
		float parentY = gameObject->parent->transform->position.y;

		position.x = parentX + Transform::position.x;
		position.y = parentY + Transform::position.y;
	}
	else {
		position.x = Transform::position.x;
		position.y = Transform::position.y;
	}

	return (point.x >= position.x && point.x <= position.x + scale.x * textureSize.x &&
		point.y >= position.y && point.y <= position.y + scale.y * textureSize.y);
}

Vector2D<float> Transform::getScale() const {
	return scale;
}

Vector2D<float> Transform::getPosition() const {
	return position;
}

// Actualiza la posición y el rectángulo destino
void Transform::setPosition(Vector2D<float> newPosition) {
	position = newPosition;

	// Si el objeto es hijo, pues se toman las coordenadas del padre
	if (gameObject->parent != nullptr && gameObject->parent->transform != nullptr) {
		float parentX = gameObject->parent->transform->position.x;
		float parentY = gameObject->parent->transform->position.y;

		dstRect.x = parentX + position.x - dstRect.w / 2;
		dstRect.y = parentY + position.y - dstRect.h / 2;
	}
	else {
		dstRect.x = position.x - dstRect.w / 2;
		dstRect.y = position.y - dstRect.h / 2;
	}
}

// Actualiza la escala y el tamaño del rectángulo destino
void Transform::setScale(Vector2D<float> newScale) {
	scale = newScale;
	dstRect.w = newScale.x;
	dstRect.h = newScale.y;
}

// Actualiza el tamaño del rectángulo destino basado en el tamaño de la textura y la escala
void Transform::updateTextureSize(Vector2D<float> size) {
	textureSize = size;
	dstRect.w = textureSize.x * scale.x;
	dstRect.h = textureSize.y * scale.y;

	// Si el objeto es hijo, pues se toman las coordenadas del padre
	if (gameObject->parent != nullptr && gameObject->parent->transform != nullptr) {
		float parentX = gameObject->parent->transform->position.x;
		float parentY = gameObject->parent->transform->position.y;

		dstRect.x = parentX + position.x - dstRect.w / 2;
		dstRect.y = parentY + position.y - dstRect.h / 2;
	}
	else {
		dstRect.x = position.x - dstRect.w / 2;
		dstRect.y = position.y - dstRect.h / 2;
	}
}

void Transform::onComponentAdd() {
	gameObject->transform = this;
}