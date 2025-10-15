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
	int fontSize;

	SDL_Surface* surface;
	Texture* texture;
	TTF_Font* font;
	SDL_Renderer* renderer;
	SDL_Texture* textureSDL;

	void updateSurface();

public:
	Text();
	Text(std::string text, SDL_Color newColor, TTF_Font* font, int newFontSize, SDL_Renderer* renderer);
	~Text();

	void onComponentAdd() override;

	void setText(const std::string& newText);
	void setFontSize(int newFontSize);
	void setColor(SDL_Color newColor);
};