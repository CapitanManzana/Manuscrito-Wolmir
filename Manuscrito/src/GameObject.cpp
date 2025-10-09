#include "GameObject.h"

int GameObject::idCounter = 0;

#pragma region Constructores
GameObject::GameObject() {
	position = Vector2D<float>(0.0f, 0.0f);
	size = Vector2D<float>(1.0f, 1.0f);
	name = "Default";
	texture = nullptr;
	id = idCounter++;
	isActive = true;

	row = 0;
	col = 0;

	dstRect = { position.getX(), position.getY(), size.getX(), size.getY()};
}

GameObject::GameObject(Vector2D<float> position, Vector2D<float> size, Texture* texture) {
	this->position = position;
	this->size = size;
	this->texture = texture;
	name = "Default";
	id = idCounter++;
	isActive = true;

	row = 0;
	col = 0;

	dstRect = { position.getX(), position.getY(), size.getX(), size.getY() };
}

GameObject::GameObject(std::string name, Vector2D<float> position, Vector2D<float> size, Texture* texture) {
	this->position = position;
	this->size = size;
	this->texture = texture;
	this->name = name;
	id = idCounter++;
	isActive = true;

	row = 0;
	col = 0;

	dstRect = { position.getX(), position.getY(), size.getX(), size.getY() };
}

GameObject::GameObject(Vector2D<float> position, Vector2D<float> size, Texture* texture, int row, int col) {
	this->position = position;
	this->size = size;
	this->texture = texture;
	name = "Default";
	id = idCounter++;
	isActive = true;

	GameObject::row = row;
	GameObject::col = col;

	dstRect = { position.getX(), position.getY(), size.getX(), size.getY() };
}

GameObject::GameObject(std::string name, Vector2D<float> position, Vector2D<float> size, Texture* texture, int row, int col) {
	this->position = position;
	this->size = size;
	this->texture = texture;
	this->name = name;
	id = idCounter++;
	isActive = true;

	GameObject::row = row;
	GameObject::col = col;

	dstRect = { position.getX(), position.getY(), size.getX(), size.getY() };
}
#pragma endregion

GameObject::~GameObject() {
	texture = nullptr; // No se elimina la textura, ya que puede ser compartida
}

void GameObject::render() {
	if (isActive && texture != nullptr) {
		texture->renderFrame(dstRect, row, col);
	}
}

Vector2D<float> GameObject::getSize() const{
	return size;
}

Vector2D<float> GameObject::getPosition() const {
	return position;
}

void GameObject::setPosition(Vector2D<float> newPosition) {
	position = newPosition;
	dstRect.x = position.getX();
	dstRect.y = position.getY();
}

std::string GameObject::getName() const {
	return name;
}

int GameObject::getId() const {
	return id;
}

bool GameObject::getIsActive() const { 
	return isActive; 
}
void GameObject::setIsActive(bool active) { 
	isActive = active; 
}