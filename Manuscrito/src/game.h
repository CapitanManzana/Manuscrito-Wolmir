#ifndef GAME_H
#define GAME_H

#include <SDL3/SDL.h>
#include <array>
#include <istream>
#include <vector>

// Declaraciones anticipadas
class Texture;
class GameObject;
class Book;

/**
 * Clase principal del juego.
 */
class Game
{
public:
	// Se actualiza el juego cada tantos milisegundos
	static constexpr int FRAME_RATE = 60;
	// Tamaño real de la ventana
	static constexpr int WINDOW_WIDTH = 1920/2;
	static constexpr int WINDOW_HEIGHT = 1080/2;
	// Extremo inferior del río
	static constexpr int RIVER_LOW = 210;

	enum TextureName
	{
		BACKGROUND,
		CHINO,
		HOJA1,
		HOJA3,
		HOJA4,
		HOJA5,
		HOJA_VACIA,
		NUM_TEXTURES
	};

private:
	SDL_Window* window;
	SDL_Renderer* renderer;
	std::array<Texture*, NUM_TEXTURES> textures;

	void render() const;
	void update();
	void handleEvents();
	void createGameObjects();

	bool exit;

	static std::vector<GameObject*> gameObjects;

	Uint64 perfFrequency; // Frecuencia del contador de alto rendimiento (ticks por segundo)
	Uint64 lastTime;

public:
	Game();
	~Game();

	// Obtiene una textura por su nombre
	Texture* getTexture(TextureName name) const;

	// Ejecuta el bucle principal del juego
	void run();

	// Comprueba si hay algún objeto colocado en ese rectángulo
	bool checkCollision(const SDL_FRect& rect) const;
};

inline Texture*
Game::getTexture(TextureName name) const
{
	return textures[name];
}

#endif // GAME_H
