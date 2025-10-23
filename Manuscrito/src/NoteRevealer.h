#pragma once
#include "Component.h"

class Notebook;

class NoteRevealer : public Component
{
public:
	NoteRevealer(int noteIndex, Notebook* nb);
	Notebook* notebook = nullptr;
	int noteIndex;
};

