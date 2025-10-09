#pragma once
#include "Component.h"
#include <functional>

class Button : public Component
{
public:
	Button() = default;
	virtual ~Button() = default;

	using OnClickAction = std::function<void()>;

	// Almacenamos la función que se llamará al hacer clic.
	OnClickAction onClick;
};

