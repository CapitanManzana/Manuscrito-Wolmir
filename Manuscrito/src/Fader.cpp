#include "Fader.h"
#include "SpriteRenderer.h"
#include "GameObject.h"
#include "texture.h"

Fader::Fader(float fadeDuration) {
	fadeInEnd = false;
	Fader::fadeDuration = fadeDuration;
	texture = nullptr;

	fadeIn = false;
	fadeOut = false;
	onAnimation = true;
}

Fader::Fader(float fadeDuration, bool fadeInFirst) {
	fadeInEnd = false;
	Fader::fadeDuration = fadeDuration;
	texture = nullptr;

	fadeIn = fadeInFirst;
	fadeOut = !fadeIn;
	onAnimation = true;
}

void Fader::Update(float deltaTime) {
	// Hace el fade in, sino el fade out solo si se ha idnicado que empiece
	if (fadeIn && !fadeInEnd) {
		elapsedTime += deltaTime;
		if (elapsedTime > fadeDuration) elapsedTime = fadeDuration;

		// Curva de aceleración (ease-in)
		float t = elapsedTime / fadeDuration;
		float eased = 1 - pow(1 - t, 3);
		Uint8 alpha = static_cast<Uint8>(255 * eased);

		if (alpha >= 255.0f) {
			alpha = 255.0f;
			fadeInEnd = true;
			elapsedTime = 0;
			onAnimation = false;
		}

		SDL_SetTextureAlphaMod(texture, alpha);
	}
	else if (fadeIn && fadeInEnd) {
		if (onFadeInEnd) {
			onFadeInEnd();
		}
		fadeInEnd;
	}

	if (fadeOut && !fadeOutEnd) {
		elapsedTime += deltaTime;
		if (elapsedTime > fadeDuration) elapsedTime = fadeDuration;

		float t = elapsedTime / fadeDuration;
		float eased = 1 - pow(1 - t, 3);
		Uint8 alpha = static_cast<Uint8>(255 * (1.0f - eased)); // de 255 → 0

		if (alpha <= 0) {
			alpha = 0;
			fadeOutEnd = true;
			elapsedTime = 0;
			onAnimation = false;
		}

		SDL_SetTextureAlphaMod(texture, alpha);
	}
	else if (fadeOut && fadeOutEnd) {
		if (onFadeOutEnd) {
			onFadeOutEnd();
		}
		fadeOut = false;
	}
}

void Fader::startFadeOut() {
	if (!onAnimation) {
		fadeOut = true;
	}
}

void Fader::startFadeIn() {
	if (!onAnimation) {
		fadeIn = true;
	}
}

void Fader::onComponentAdd() {
	Texture* tx = gameObject->spriteRenderer->getTexture();
	if (tx) {
		texture = tx->getTexture();
		SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
		if (fadeIn) {
			SDL_SetTextureAlphaMod(texture, 0);
		}
	}
}