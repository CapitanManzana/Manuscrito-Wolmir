#pragma once
#include "Component.h"
#include <functional>
#include "SDL3/SDL.h"

class Hover : public Component
{
private:
	bool isHovering = false;
	SDL_Renderer* renderer = nullptr;
public:
	using OnHoverAction = std::function<void()>;

	Hover(SDL_Renderer* renderer) : renderer(renderer) {}
	Hover(OnHoverAction actionIn, OnHoverAction actionStay, OnHoverAction actionOut) : onEnterHover(std::move(actionIn)), onStayHover(std::move(actionStay)), onExitHover(std::move(actionOut)) {}
	virtual ~Hover() = default;

	void Update(float deltaTime) override;

	// Almacenamos la función que se llamará al hacer clic.
	OnHoverAction onEnterHover;
	OnHoverAction onExitHover;
	OnHoverAction onStayHover;
private:
	bool checkCursorOver();
};

