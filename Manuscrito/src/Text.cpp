#include "Text.h"
#include "GameObject.h"
#include "texture.h"

#pragma region Constructores y destructores

Text::Text() {
	this->text = "";
	this->color = { 255, 255, 255, 255 };
	this->fontSize = 40;

	texture = nullptr;
	surface = nullptr;
	font = nullptr;
	renderer = nullptr;
	textureSDL = nullptr;
}

Text::Text(std::string text, SDL_Color newColor, TTF_Font* font, int newFontSize, SDL_Renderer* renderer) {
	this->text = text;
	this->color = newColor;
	this->fontSize = newFontSize;
	this->font = font;
	this->renderer = renderer;
}

Text::~Text() {
	if (surface) {
		SDL_DestroySurface(surface);
		surface = nullptr;
	}

	delete texture;

	if (textureSDL) {
		SDL_DestroyTexture(textureSDL);
		textureSDL = nullptr;
	}

	font = nullptr;
	texture = nullptr;
	renderer = nullptr;
}

#pragma endregion

void Text::updateSurface() {
	if (surface) {
		SDL_DestroySurface(surface);
		surface = nullptr;
	}

	surface = TTF_RenderText_Blended(font, text.c_str(), 0, color);
	if (!surface) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create text surface: %s", SDL_GetError());
	}
}

void Text::onComponentAdd() {
	if (!font) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No font assigned to Text component in GameObject %s", gameObject->getName());
		return;
	}

	if (!renderer) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No renderer assigned to Text component in GameObject %s", gameObject->getName());
		return;
	}

	updateSurface();

	if (surface) {
		textureSDL = SDL_CreateTextureFromSurface(renderer, surface);
		if (!textureSDL) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create text texture: %s", SDL_GetError());
		}
		else {
			texture = new Texture(renderer, textureSDL, 1, 1);
			if (gameObject->spriteRenderer) {
				gameObject->spriteRenderer->setTexture(texture);
			}
			else {
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No SpriteRenderer component in GameObject %s to assign the text texture", gameObject->getName());
			}
		}

		if (texture && gameObject->transform) {
			gameObject->transform->updateTextureSize(Vector2D<float>(surface->w, surface->h));
		}

		// Liberamos la superficie ya que no la necesitamos más
		SDL_DestroySurface(surface);
		surface = nullptr;
	}
}

#pragma region Setters
void Text::setText(const std::string& newText) {
	if (text != newText) {
		text = newText;
		updateSurface();
	}
}
void Text::setFontSize(int newFontSize) {
	if (fontSize != newFontSize) {
		fontSize = newFontSize;
		updateSurface();
	}
}
void Text::setColor(SDL_Color newColor) {
	if (color.r != newColor.r || color.g != newColor.g || color.b != newColor.b || color.a != newColor.a) {
		color = newColor;
		updateSurface();
	}
}
#pragma endregion
