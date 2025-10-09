#pragma once
#include <string>
#include <vector>
#include "SpriteRenderer.h"
#include "Transform.h"

class GameObject
{
private:
	std::string name;
	int id;

	std::vector<std::unique_ptr<Component>> components;
	bool isActive;

	static int idCounter;
public:
	Transform* transform = nullptr;
	// Añadimos un puntero al SpriteRenderer para delegar el render
	SpriteRenderer* spriteRenderer = nullptr;

	GameObject();
	GameObject(std::string name, Transform* transform, SpriteRenderer* sprite, size_t nComponents);
	~GameObject() = default;

	void update(float deltaTime);
	void render(); // El render ahora lo delegaremos al SpriteComponent

	bool getIsActive() const;
	void setIsActive(bool active);

	std::string getName() const;
	int getId() const;

public:
	template<typename T, typename... TArgs>
	T* addComponent(TArgs&&... args) { // <-- CAMBIO: Devuelve T* en lugar de T&
		auto comp = std::make_unique<T>(std::forward<TArgs>(args)...);
		comp->gameObject = this;

		T* rawPtr = comp.get(); // Obtenemos el puntero antes de moverlo

		components.push_back(std::move(comp));

		return rawPtr; // <-- CAMBIO: Devuelve el puntero
	}

	template<typename T>
	T* getComponent() const {
		for (const auto& comp : components) {
			if (auto castedComp = dynamic_cast<T*>(comp.get())) {
				return castedComp;
			}
		}
		return nullptr;
	}
};