#pragma once
#include <SDL3/SDL.h>
#include <functional>
#include "Component.h"

class SpriteRenderer;

class Fader : public Component
{
private:
	float fadeDuration = 3.0f;
	float alpha = 0.0f;
	float elapsedTime = 0.0f;

	bool fadeInEnd = false;
	bool fadeOutEnd = false;
	bool fadeOut = false;
	bool fadeIn = false;

	bool onAnimation = false;
	bool fadeInFirst = false;

	SDL_Texture* texture;

public:
	Fader() = default;
	Fader(float fadeDuration);
	Fader(float fadeDuration, bool fadeInFirst);

	using OnFadeOutEnd = std::function<void()>;


	void Update(float deltaTime) override;
	void onComponentAdd() override;
	void startFadeOut();
	void startFadeIn();

	void resetFade();

	OnFadeOutEnd onFadeOutEnd;
	OnFadeOutEnd onFadeInEnd;
};

