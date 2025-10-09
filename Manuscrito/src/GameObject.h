#pragma once
#include "texture.h"
#include "vector2D.h"
#include "Component.h"
#include <string>
#include <vector>
#include "SpriteRenderer.h"

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
	GameObject(std::string name, Transform* transform, SpriteRenderer* sprite);
	~GameObject() = default;

	void update(float deltaTime);
	void render(); // El render ahora lo delegaremos al SpriteComponent

	bool getIsActive() const;
	void setIsActive(bool active);

	template<typename T, typename... TArgs>
	T& addComponent(TArgs&&... args);

	template<typename T>
	T* getComponent() const;

	std::string getName() const;
	int getId() const;

	bool getIsActive() const;
	void setIsActive(bool active);
};