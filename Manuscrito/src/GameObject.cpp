#include "GameObject.h"
#include <SDL3/SDL.h>  // Necesita SDL_FRect

int GameObject::idCounter = 0;

#pragma region Constructores
GameObject::GameObject() {

	components.reserve(2);

    transform = addComponent<Transform>();
	spriteRenderer = addComponent<SpriteRenderer>();

    name = "GameObject_" + std::to_string(idCounter);
    id = idCounter++;
    isActive = true;
}

GameObject::GameObject(std::string name, size_t nComponents) {
	components.reserve(nComponents);

	transform = addComponent<Transform>();
	spriteRenderer = addComponent<SpriteRenderer>();

	GameObject::name = name;
	id = idCounter++;
	isActive = true;
}

GameObject::GameObject(std::string name, Transform* transform, SpriteRenderer* sprite, size_t nComponents){
	components.reserve(nComponents);

	// ASIGNACION DE TRANSFORM
    GameObject::transform = transform;
	if (transform == nullptr) {
		GameObject::transform = addComponent<Transform>();
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Transform nulo en GameObject %s. Se ha creado uno por defecto.", name.c_str());
	}
	else {
		transform->gameObject = this; // Asegura que el Transform conoce su GameObject
		components.push_back(std::unique_ptr<Transform>(transform));
	}

	// ASIGNACION DE SPRITERENDERER
	spriteRenderer = sprite;
	if (spriteRenderer == nullptr) {
		spriteRenderer = addComponent<SpriteRenderer>();
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "SpriteRenderer nulo en GameObject %s. Se ha creado uno por defecto.", name.c_str());
	}
	else {
		spriteRenderer->gameObject = this; // Asegura que el SpriteRenderer conoce su GameObject
		components.push_back(std::unique_ptr<SpriteRenderer>(spriteRenderer));
	}

	GameObject::name = name;
	id = idCounter++;
	isActive = true;
}

GameObject::~GameObject() {
	for (auto& comp : components) {
		comp->OnDestroy(); // Llama a OnDestroy de cada componente
	}

	transform = nullptr;
	spriteRenderer = nullptr;
	components.clear();
}

#pragma endregion

void GameObject::update(float deltaTime) {
    if (!isActive) return;
    for (auto& comp : components) {
        comp->DoUpdate(deltaTime); // Llama a Start/Update de cada componente
    }
}

void GameObject::render() {
    if (!isActive) return;
    // La responsabilidad de renderizar ahora es del SpriteComponent
    if (spriteRenderer) {
        spriteRenderer->render();
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
	isActive = active; 
}
#pragma endregion
