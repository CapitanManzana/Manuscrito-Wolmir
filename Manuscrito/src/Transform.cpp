#include "Transform.h"
#include "GameObject.h"

#pragma region Constructores

Transform::Transform() {
	position = Vector2D<float>(0.0f, 0.0f);
	scale = Vector2D<float>(1.0f, 1.0f);

	dstRect = { position.x, position.y, scale.x, scale.y };
	parentTransform = nullptr;
}

Transform::Transform(Vector2D<float> position, Vector2D<float> size) {
	this->position = position;
	scale = size;

	dstRect = { position.x, position.y, scale.x, scale.y };

	scaleMode = ScaleMode::DEFAULT;
	parentTransform = nullptr;
}

Transform::Transform(Vector2D<float> position, float size) {
	this->position = position;
	scale = {
		size,
		size
	};

	dstRect = { position.x, position.y, scale.x, scale.y };

	scaleMode = ScaleMode::SCALAR;
	parentTransform = nullptr;
}

#pragma endregion

Vector2D<float> Transform::getScale() const {
	return scale;
}

Vector2D<float> Transform::getPosition() const {
	return position;
}

Vector2D<float> Transform::getGlobalPosition() const {
	if (parentTransform == nullptr) {
		return position;
	}
	else {
		return parentTransform->getGlobalPosition() + position;
	}
}

// Actualiza la posici�n y el rect�ngulo destino
void Transform::setPosition(Vector2D<float> newPosition) {
	position = newPosition;

	if (parentTransform != nullptr) {
		Vector2D<float> parentPos = parentTransform->getGlobalPosition();
		dstRect.x = parentPos.x + position.x - dstRect.w / 2;
		dstRect.y = parentPos.y + position.y - dstRect.h / 2;
	}
	else {
		dstRect.x = position.x - dstRect.w / 2;
		dstRect.y = position.y - dstRect.h / 2;
	}
}

// Actualiza la escala y el tama�o del rect�ngulo destino
void Transform::setScale(Vector2D<float> newScale) {
	scale = newScale;
	if (scaleMode == ScaleMode::SCALAR) {
		dstRect.w = textureSize.x * scale.x;
		dstRect.h = textureSize.y * scale.y;
	}
	else {
		dstRect.w = newScale.x;
		dstRect.h = newScale.y;
	}
}

void Transform::setScaleMode(ScaleMode mode) {
	scaleMode = mode;
}

// Actualiza el tama�o del rect�ngulo destino basado en el tama�o de la textura y la escala
void Transform::updateTextureSize(Vector2D<float> size) {
	textureSize = size;

	if (scaleMode == ScaleMode::SCALAR) {
		dstRect.w = textureSize.x * scale.x;
		dstRect.h = textureSize.y * scale.y;
	}

	// Si el objeto es hijo, pues se toman las coordenadas del padre
	if (gameObject->parent != nullptr && gameObject->parent->transform != nullptr) {
		Vector2D<float> parentPos = parentTransform->getGlobalPosition();

		dstRect.x = parentPos.x + position.x - dstRect.w / 2;
		dstRect.y = parentPos.y + position.y - dstRect.h / 2;
	}
	else {
		dstRect.x = position.x - dstRect.w / 2;
		dstRect.y = position.y - dstRect.h / 2;
	}
}

void Transform::onComponentAdd() {
	gameObject->transform = this;

	if (gameObject->parent != nullptr) parentTransform = gameObject->parent->transform;
	setPosition(position);
	updateTextureSize(scale);
}