#include "GameObject.h"
#include <SDL3/SDL.h>  // Necesita SDL_FRect

int GameObject::idCounter = 0;

#pragma region Constructores
GameObject::GameObject() {

	components.reserve(2);
	spriteRenderer = nullptr;
	transform = nullptr;

	name = "GameObject_" + std::to_string(idCounter);
	id = idCounter++;
	isActive = true;
}

GameObject::GameObject(std::string name, size_t nComponents) {
	components.reserve(nComponents);
	spriteRenderer = nullptr;
	transform = nullptr;

	GameObject::name = name;
	id = idCounter++;
	isActive = true;
}

GameObject::GameObject(std::string name, size_t nComponents, GameObject* parent) {
	components.reserve(nComponents);
	spriteRenderer = nullptr;
	transform = nullptr;

	this->parent = parent;
	parent->childs.push_back(this);

	GameObject::name = name;
	id = idCounter++;
	isActive = true;
}

GameObject::~GameObject() {
	for (auto& comp : components) {
		comp->OnDestroy(); // Llama a OnDestroy de cada componente
	}

	components.clear();
}

#pragma endregion

void GameObject::start() {
	for (auto& c : components) {
		c->Start();
	}
}

void GameObject::update(float deltaTime) {
	if (!isActive) return;
	for (auto& comp : components) {
		comp->DoUpdate(deltaTime); // Llama a Start/Update de cada componente
	}
}

void GameObject::render() {
	if (!isActive) return;

	// La responsabilidad de renderizar ahora es del SpriteComponent
	if (spriteRenderer != nullptr) {
		try {
			spriteRenderer->render();
		}
		catch (const std::runtime_error& e) {
			SDL_Log("Se produjo un error: %s", e.what());
		}
	}
}

#pragma region Getters y Setters

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
	if (childs.size() > 0) {
		for (GameObject* child : childs) {
			child->setIsActive(active);
		}
	}

	isActive = active;
}

GameObject* GameObject::getChild(int index) {
	if (index < 0 || index >= childs.size()) {
		return nullptr; // O lanza una excepción si prefieres
	}
	return childs[index];
}

std::vector<GameObject*> GameObject::getChildren() {
	return childs;
}

#pragma endregion
