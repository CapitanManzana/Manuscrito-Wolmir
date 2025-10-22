#pragma once
#include <vector>
#include <iostream>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

class GameObject;
class Texture;

class Notebook
{
private:
	const int MAIN_FONT_SIZE = 90;
	const int SECONDARY_FONT_SIZE = 60;
	const int MAIN_TEXT_WIDTH = 700;
	const int SECONDARY_TEXT_WIDTH = 500;

	const int NOTE_SPACING = 10;

	SDL_Color mainNoteColor = { 196, 26, 0, 255 }; // Dorado
	SDL_Color secondaryNoteColor = { 0, 0, 0, 255 }; // Negro
	SDL_Renderer* renderer;
	TTF_Font* font;

	Texture* marcoTexture;

	GameObject* notebookObject;
	std::vector<GameObject*> notes;
	std::vector<GameObject*> totalObjects;
	int notesCount;
	int discoveredNotes;

public:
	Notebook(std::istream& is, GameObject* parent, TTF_Font* font, Texture* tex, SDL_Renderer* rend);
	~Notebook();

	// Revela una nota en el cuaderno (un descubrimiento)
	void revealNote(int index);
	void render() const;

	GameObject* getNote(int index) const;
	std::vector<GameObject*> getNotes() const { return notes; }
};

