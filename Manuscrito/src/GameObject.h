#pragma once
#include "texture.h"
#include "vector2D.h"
#include "MonoBehaviour.h"
#include <string>

class GameObject : public MonoBehaviour
{
private:
	// Posición y tamaño del objeto en la pantalla
	Vector2D<float> position;
	Vector2D<float> size;

	// Fila y columna actuales para animaciones
	int row;
	int col;

	// Rectángulo destino para renderizar con SDL
	SDL_FRect dstRect;

	// Textura del objeto
	Texture* texture;
	std::string name;
	int id;
	bool isActive;

	static int idCounter;

public:
	GameObject();
	~GameObject();
	GameObject(Vector2D<float> position, Vector2D<float> size, Texture* texture);
	GameObject(Vector2D<float> position, Vector2D<float> size, Texture* texture, int row, int col);
	GameObject(std::string name, Vector2D<float> position, Vector2D<float> size, Texture* texture);
	GameObject(std::string name, Vector2D<float> position, Vector2D<float> size, Texture* texture, int row, int col);

	Vector2D<float> getSize() const;
	Vector2D<float> getPosition() const;
	void setPosition(Vector2D<float> newPosition);
	std::string getName() const;
	int getId() const;

	bool getIsActive() const;
	void setIsActive(bool active);

	void render();

	static GameObject* findById(int id);
};