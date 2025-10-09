#pragma once
#include "MonoBehaviour.h"

class Component : public MonoBehaviour
{
public:
	bool isEnabled = true;
	virtual ~Component() = default;
};

