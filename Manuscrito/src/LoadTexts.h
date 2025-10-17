#pragma once
#include <iostream>

#include <string>
#include <vector>
#include <map>
#include "Vector2D.h"

#include <SDL3/SDL_Rect.h>

struct TextData
{
	std::string text;
	Vector2D<int> position;
	int textEnd;
	SDL_FRect rect;

	TextData(std::string text, SDL_FRect rect, Vector2D<int> position, int textEnd) {
		this->rect = rect;
		this->text = text;
		this->position = position;
		this->textEnd = textEnd;
	}

	TextData() {
		text = "";
		rect = { 0,0,0,0 };
		position = { 0,0 };
		textEnd = 0;
	}
};

class LoadTexts
{
private:
	std::map<size_t, std::map<size_t, TextData>> data;
public:
	LoadTexts(std::string filePath);

	TextData getTextData(size_t page, size_t index);
	int getTextsCount(size_t page);
};