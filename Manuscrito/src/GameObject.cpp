#include "GameObject.h"

int GameObject::idCounter = 0;

#pragma region Constructores
GameObject::GameObject() {
    transform = &addComponent<Transform>();
	spriteRenderer = &addComponent<SpriteRenderer>();

    name = "GameObject_" + std::to_string(idCounter);
    id = idCounter++;
    isActive = true;
}

GameObject::GameObject(std::string name, Transform* transform, SpriteRenderer* sprite){
    GameObject::transform = transform;

	if (transform == nullptr) {
		GameObject::transform = &addComponent<Transform>();
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Transform nulo en GameObject %s. Se ha creado uno por defecto.", name.c_str());
	}

	spriteRenderer = sprite;
	if (spriteRenderer == nullptr) {
		spriteRenderer = &addComponent<SpriteRenderer>();
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "SpriteRenderer nulo en GameObject %s. Se ha creado uno por defecto.", name.c_str());
	}

	GameObject::name = name;
	id = idCounter++;
	isActive = true;
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

// Las funciones addComponent y getComponent se quedan como estaban
template<typename T, typename... TArgs>
T& GameObject::addComponent(TArgs&&... args) {
    auto comp = std::make_unique<T>(std::forward<TArgs>(args)...);
    comp->gameObject = this;
    components.push_back(std::move(comp));
    return *dynamic_cast<T*>(components.back().get());
}

template<typename T>
T* GameObject::getComponent() const {
    for (const auto& comp : components) {
        if (auto castedComp = dynamic_cast<T*>(comp.get())) {
            return castedComp;
        }
    }
    return nullptr;
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
