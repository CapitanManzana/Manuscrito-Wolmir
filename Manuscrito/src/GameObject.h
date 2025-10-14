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
	std::vector<GameObject*> childs;
	bool isActive;

	static int idCounter;
public:
	SpriteRenderer* spriteRenderer; // Componente de renderizado
	Transform* transform; // Componente de transformación
	GameObject* parent = nullptr; 

	GameObject();
	GameObject(std::string name, size_t nComponents);
	GameObject(std::string name, size_t nComponents, GameObject* parent);
	~GameObject();

	void update(float deltaTime);
	void render(); // El render ahora lo delegaremos al SpriteComponent

	bool getIsActive() const;
	void setIsActive(bool active);

	std::string getName() const;
	int getId() const;

	GameObject* getChildren(int index);

public:
	template<typename T, typename... TArgs>
	T* addComponent(TArgs&&... args) { // <-- CAMBIO: Devuelve T* en lugar de T&
		auto comp = std::make_unique<T>(std::forward<TArgs>(args)...);
		comp->gameObject = this;

		T* rawPtr = comp.get(); // Obtenemos el puntero antes de moverlo

		components.push_back(std::move(comp));
		rawPtr->onComponentAdd();
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