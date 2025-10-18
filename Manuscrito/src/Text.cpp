#include "Text.h"
#include "GameObject.h"
#include "texture.h"

#pragma region Constructores y destructores

Text::Text() {
	this->text = "";
	this->color = { 255, 255, 255, 255 };
	this->textureWidth = 200;

	texture = nullptr;
	surface = nullptr;
	font = nullptr;
	renderer = nullptr;
	textureSDL = nullptr;
}

Text::Text(std::string text, SDL_Color newColor, TTF_Font* font, int width, SDL_Renderer* renderer) {
	this->text = text;
	this->color = newColor;
	this->font = font;
	this->renderer = renderer;
	this->textureWidth = width;

	surface = nullptr;
	texture = nullptr;
	textureSDL = nullptr;
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

	if (texture) {
		delete texture;
	}

	surface = TTF_RenderText_Blended_Wrapped(font, text.c_str(), 0, color, textureWidth);
	if (!surface) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create text surface: %s", SDL_GetError());
	}

	//2. Crear la textura a partir de la superficie
	//3. Asignar la textura al SpriteRenderer del GameObject
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

void Text::onComponentAdd() {
	if (!font) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No font assigned to Text component in GameObject %s", gameObject->getName());
		return;
	}

	if (!renderer) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No renderer assigned to Text component in GameObject %s", gameObject->getName());
		return;
	}

	//1. Actualizar la superficie con el texto, color y fuente actuales
	updateSurface();
}

void Text::changeFont(TTF_Font* newFont) {
	if (font != newFont) {
		font = newFont;
		updateSurface();
	}
}

void Text::changeFont(TTF_Font* newFont, size_t size) {
	if (font != newFont) {
		font = newFont;

		updateSurface();
	}
}

#pragma region Setters
void Text::setText(const std::string& newText) {
	if (text != newText) {
		text = newText;
		updateSurface();
	}
}

void Text::setColor(SDL_Color newColor) {
	if (color.r != newColor.r || color.g != newColor.g || color.b != newColor.b || color.a != newColor.a) {
		color = newColor;
		updateSurface();
	}
}

SDL_Color Text::getColor() const{
	return color;
}
#pragma endregion
