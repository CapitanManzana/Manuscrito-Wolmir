#pragma once
#include "Component.h"
#include <string>

class Text;

class NotebookElement : public Component
{
private:
	std::string title;

public:
	NotebookElement(const std::string& title);
	bool discovered = false;

	void reveal();
};

