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

	texture = nullptr;
	surface = nullptr;
	font = nullptr;
	renderer = nullptr;
	textureSDL = nullptr;
	currentText = "";
	fontSize = 16;
}

Text::Text(std::string text, SDL_Color newColor, TTF_Font* font, int width, int size, SDL_Renderer* renderer) {
	this->text = text;
	currentText = text;
	this->color = newColor;
	this->font = font;
	this->renderer = renderer;
	this->textureWidth = width;

	surface = nullptr;
	texture = nullptr;
	textureSDL = nullptr;

	TTF_SetFontSize(font, size);
	fontSize = size;
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

void Text::updateSurface() {
	if (surface) {
		SDL_DestroySurface(surface);
		surface = nullptr;
	}

	if (texture) {
		delete texture;
	}

	surface = TTF_RenderText_Blended_Wrapped(font, currentText.c_str(), 0, color, textureWidth);
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
	if (color.r != newColor.r || color.g != newColor.g || color.b != newColor.b || color.a != newColor.a) {
		color = newColor;
		updateSurface();
	}
}

SDL_Color Text::getColor() const{
	return color;
}

int Text::getFontSize() const {
	return fontSize;
}
#pragma endregion
