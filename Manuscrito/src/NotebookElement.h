#pragma once
#include "Component.h"
#include <string>

class Text;

class NotebookElement : public Component
{
private:
	std::string title;
	int fontSize = 16;

public:
	NotebookElement(const std::string& title, int fSize);
	bool discovered = false;

	void reveal();
};

