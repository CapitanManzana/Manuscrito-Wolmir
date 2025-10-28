#include "Text.h"
#include "GameObject.h"
#include "texture.h"
#include "game.h"
#include "NoteRevealer.h"
#include "Notebook.h"

bool Text::showingText = false;

#pragma region Constructores y destructores

Text::Text() {
	this->text = "";
	this->color = { 255, 255, 255, 255 };
	this->textureWidth = 200;

	font = nullptr;
	renderer = nullptr;
	currentText = "";
	fontSize = 16;

	timePerChar = 0;
}

Text::Text(std::string text, SDL_Color newColor, TTF_Font* font, int width, int size, SDL_Renderer* renderer) {
	this->text = text;
	currentText = text;
	this->color = newColor;
	this->font = font;
	this->renderer = renderer;
	this->textureWidth = width;

	timePerChar = 0;

	TTF_SetFontSize(font, size);
	fontSize = size;
}

#pragma endregion

void Text::Update(float deltaTime) {
	if (showText) {
		// 1. Comprobar si aún quedan caracteres por mostrar
		if (charIndex < text.length()) {
			if (!startedTyping) {
				currentText = "";
				font = Game::baseFont;

				TTF_SetFontSize(font, fontSize - 16);
				TTF_SetFontWrapAlignment(font, TTF_HORIZONTAL_ALIGN_LEFT);

				startedTyping = true;
				showingText = true;

				if (text.length() == 0) {
					timePerChar = 0.0f;
					return;
				}

				timePerChar = totalTypingTime / (float)text.length();
			}
			// 2. Acumular el tiempo
			timer += deltaTime;
			// 3. Si ha pasado el tiempo necesario para el siguiente carácter
			if (timer >= timePerChar) {
				// 4. Añadir el siguiente carácter al texto visible
				currentText += text[charIndex];
				// 5. Avanzar al siguiente índice
				charIndex++;
				// 6. Restar el tiempo consumido (esto es más preciso que timer = 0)
				timer -= timePerChar;
				// 7. Actualizar la superficie y textura con el nuevo texto
				updateSurface();
			}
		}
		else {
			showText = false;
			showingText = false;
			NoteRevealer* nr = gameObject->getComponent<NoteRevealer>();
			if (nr) {
				nr->notebook->discoverNote(nr->noteIndex);
			}
		}
	}
}

void Text::onComponentAdd() {
	if (!font) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No hay fuete asignada a %s", gameObject->getName());
		return;
	}

	if (!renderer) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Noy hay render asignado a %s", gameObject->getName());
		return;
	}

	//1. Actualizar la superficie con el texto, color y fuente actuales
	updateSurface();
}

void Text::updateSurface() {
	SDL_Surface* localSurface = nullptr;
	SDL_Texture* localTextureSDL = nullptr;
	Texture* newTexture = nullptr;

	localSurface = TTF_RenderText_Blended_Wrapped(font, currentText.c_str(), 0, color, textureWidth);
	if (!localSurface) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No se pudo crear la superficie: %s", SDL_GetError());
	}

	if (localSurface) {
		localTextureSDL = SDL_CreateTextureFromSurface(renderer, localSurface);
		if (!localTextureSDL) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No se pudo crear la textura: %s", SDL_GetError());
		}
		else {
			newTexture = new Texture(renderer, localTextureSDL);
			if (gameObject->spriteRenderer) {
				// Le damos la textura al SpriteRenderer.
				// Asumimos que setTexture() borra la textura anterior
				// si ya tenía una.
				gameObject->spriteRenderer->setTexture(newTexture);
			}
			else {
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No hay SpriteRenderer component en GameObject %s", gameObject->getName());
				delete newTexture; // Si no hay renderer, hay que borrarla
			}
		}

		if (newTexture && gameObject->transform) {
			gameObject->transform->updateTextureSize(Vector2D<float>(localSurface->w, localSurface->h));
		}

		// Liberamos la superficie local
		SDL_DestroySurface(localSurface);
		localSurface = nullptr;
	}
}

#pragma region Setters
void Text::setText(const std::string& newText) {
	if (currentText != newText) {
		currentText = newText;
		updateSurface();
	}
}

void Text::setText(const std::string& newText, int size) {
	if (currentText != newText) {
		currentText = newText;
		fontSize = size;
		TTF_SetFontSize(font, fontSize);
		updateSurface();
	}
}

std::string Text::getText() const {
	return text;
}

void Text::setColor(SDL_Color newColor) {
	if (color.r == newColor.r && color.g == newColor.g && color.b == newColor.b && color.a == newColor.a) return;

	color = newColor;
	TTF_SetFontSize(font, fontSize);
	updateSurface();
}

SDL_Color Text::getColor() const {
	return color;
}

int Text::getFontSize() const {
	return fontSize;
}
#pragma endregion
