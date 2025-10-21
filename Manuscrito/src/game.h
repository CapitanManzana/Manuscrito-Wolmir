#ifndef GAME_H
#define GAME_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

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

	static constexpr int FONT_SIZE = 60;
	static constexpr float MANUS_FONT_SIZE = 76.5;

	static constexpr unsigned short RUNIC_TEST_SOLUTION = 0b000100100010; // 12
	static constexpr unsigned short RUNIC_TEST_LENGHT = 3;
	static constexpr unsigned short ASTROLOGY_TEST_SOLUTION = 0b1000011; // 7
	static constexpr unsigned short ASTROLOGY_TEST_LENGHT = 8;

	static TTF_Font* baseFont;
	static TTF_Font* manuscritoFont;

	enum TextureName
	{
		BACKGROUND,
		RUNAS,
		TABLA_RUNAS,
		SELECTOR,
		HOJA1,
		HOJA5,
		HOJA6,
		HOJA8,
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

	void showText(GameObject* text);
	void renderObjects() const;
	void createLight();

	bool exit;

	static std::vector<GameObject*> gameObjects;

	Uint64 perfFrequency; // Frecuencia del contador de alto rendimiento (ticks por segundo)
	Uint64 lastTime;

public:
	Game();
	~Game();

	// Obtiene una textura por su nombre
	Texture* getTexture(TextureName name) const;
	SDL_Renderer* getRenderer() const;
	// Ejecuta el bucle principal del juego
	void run();
};

inline Texture*
Game::getTexture(TextureName name) const
{
	return textures[name];
}

inline SDL_Renderer* Game::getRenderer() const
{
	return renderer;
}

#endif // GAME_H
