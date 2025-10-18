#pragma once
#include <iostream>

#include <string>
#include <vector>
#include <map>
#include "Vector2D.h"

#include <SDL3/SDL.h>

struct TextData
{
	std::string text;
	Vector2D<int> position;
	int textEnd;
	SDL_Color color;

	TextData(std::string text, Vector2D<int> position, int textEnd) {
		this->text = text;
		this->position = position;
		this->textEnd = textEnd;
	}

	TextData() {
		text = "";
		position = { 0,0 };
		textEnd = 0;
		color = { 0, 0, 0, 255 };
	}
};

class LoadTexts
{
private:
	std::map<size_t, std::map<size_t, TextData>> data;
	SDL_Color hexToSDLColor(const std::string& hex);

public:
	LoadTexts(std::string filePath);

	TextData getTextData(size_t page, size_t index);
	int getTextsCount(size_t page);
};