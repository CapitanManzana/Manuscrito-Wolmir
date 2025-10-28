#pragma once
#include <SDL3/SDL.h>
#include <SDL3/SDL_ttf.h>
#include "Component.h"
#include <string>

class Texture;
class Game;

class Text : public Component
{
private:
	// Tiempo total para escribir todo el texto (segundos)
	float totalTypingTime = 1.5f;
	// Velocidad de escritura del texto (segundos por caracter)
	float timePerChar;
	// Índice del caracter actual a mostrar
	int charIndex = 0;
	// Temporizador para controlar el tiempo entre caracteres
	float timer = 0.0f;
	// Indica si se ha empezado a escribir el texto
	bool startedTyping = false;

	std::string text;
	std::string currentText;
	SDL_Color color;
	int textureWidth;
	int fontSize;

	SDL_Surface* surface;
	Texture* texture;
	TTF_Font* font;
	SDL_Renderer* renderer;
	SDL_Texture* textureSDL;


	void updateSurface();

public:
	Text();
	Text(std::string text, SDL_Color newColor, TTF_Font* font, int width, int size, SDL_Renderer* renderer);
	~Text();

	static bool showingText;

	void onComponentAdd() override;

	SDL_Color getColor() const;
	int getFontSize() const;

	void Update(float deltaTime) override;

	void setText(const std::string& newText);
	void setText(const std::string& newText, int size);
	std::string getText() const;
	void setColor(SDL_Color newColor);

	bool showText = false;
};