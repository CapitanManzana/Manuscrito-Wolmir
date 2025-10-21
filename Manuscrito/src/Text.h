#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "Component.h"
#include <string>

class Texture;
class Game;

class Text : public Component
{
private:
	std::string text;
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

	void onComponentAdd() override;

	SDL_Color getColor() const;
	int getFontSize() const;

	void setText(const std::string& newText);
	void setColor(SDL_Color newColor);

	void changeFont(TTF_Font* newFont, int size);
	void changeFont(TTF_Font* newFont, size_t textureWidth);

	bool showed = false;
};