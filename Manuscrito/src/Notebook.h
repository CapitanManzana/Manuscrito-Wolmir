#pragma once
#include <vector>
#include <iostream>
#include <map>
#include <SDL3/SDL.h>
#include <SDL3/SDL_ttf.h>

class GameObject;
class Texture;
class Game;
class Scene;

class Notebook
{
private:
	const int TITLE_FONT_SIZE = 200;

	const int MAIN_FONT_SIZE = 90;
	const int SECONDARY_FONT_SIZE = 65;
	const int MAIN_TEXT_WIDTH = 700;
	const int SECONDARY_TEXT_WIDTH = 500;

	const int MIN_NOTE_HEIGHT = 50;

	const int NOTE_SPACING = 10;

	SDL_Color mainNoteColor = { 196, 26, 0, 255 }; // Dorado
	SDL_Color secondaryNoteColor = { 0, 0, 0, 255 }; // Negro
	SDL_Renderer* renderer;
	TTF_Font* font;

	Texture* marcoTexture;

	GameObject* notebookObject;
	std::map<int, std::vector<int>> noteConnections;
	std::vector<GameObject*> notes;
	std::vector<GameObject*> totalObjects;
	std::vector<GameObject*> infoObjects;
	int notesCount;
	int discoveredNotes;

	GameObject* lastInfoObject = nullptr;

	void onClickNote(int index);
public:
	Notebook(std::istream& is, GameObject* parent, TTF_Font* font, Texture* tex, SDL_Renderer* rend, Scene* scene);
	~Notebook();

	// Revela una nota en el cuaderno (un descubrimiento)
	void discoverNote(int index);
	void render() const;
	void renderLines() const;
	void update(float deltaTime);

	GameObject* getNote(int index) const;
	std::vector<GameObject*> getNotes() const { return notes; }
};

