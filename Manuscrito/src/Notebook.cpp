#include "Notebook.h"
#include "GameObject.h"
#include "Text.h"
#include "texture.h"

#include<SDL3_ttf/SDL_ttf.h>

Notebook::Notebook(std::istream& is, GameObject* parent, TTF_Font* font, Texture* tex, SDL_Renderer* rend) {
	notebookObject = parent;
	notesCount = 0;
	discoveredNotes = 0;
	renderer = rend;
	this->font = font;
	marcoTexture = tex;

	std::string tag;

	// Leemos la cantidad de notas
	is >> tag >> notesCount;
	for (int i = 0; i < notesCount; i++) {
		// Leemos NX:
		is >> tag;

		std::string title, desciption;
		Vector2D<float> position;
		int category;
		int relatedCount;

		is >> tag;
		std::getline(is, title);
		is >> tag;
		std::getline(is, desciption);
		is >> tag >> position.x;
		is >> tag >> position.y;
		is >> tag >> category;
		is >> tag >> relatedCount;

		std::vector<int> relatedNotes(relatedCount);
		for (int j = 0; j < relatedCount; j++) {
			is >> relatedNotes[j];
		}

		GameObject* noteObject = new GameObject("Note_" + std::to_string(i), 5, notebookObject);
		noteObject->addComponent<Transform>(position, 0.15);
		noteObject->addComponent<SpriteRenderer>();
		Text* text = nullptr;
		switch (category) {
		case 0: // MAIN
			text = noteObject->addComponent<Text>(title, mainNoteColor, font, MAIN_TEXT_WIDTH, MAIN_FONT_SIZE, renderer);
				break;

		case 1: // SECONDARY
			text = noteObject->addComponent<Text>(title, secondaryNoteColor, font, SECONDARY_TEXT_WIDTH, SECONDARY_FONT_SIZE, renderer);
			break;

		default:
			break;
		}

		if (text) {
			text->setHorizontalAlign(TTF_HORIZONTAL_ALIGN_CENTER);

			float w = noteObject->spriteRenderer->getTexture()->getWidth() * noteObject->transform->getScale().x + NOTE_SPACING;
			float h = noteObject->spriteRenderer->getTexture()->getHeight() * noteObject->transform->getScale().y + NOTE_SPACING;
			GameObject* marco = new GameObject("MarcoNota_" + std::to_string(i), 2, noteObject);
			marco->addComponent<Transform>(Vector2D<float>(0, 0), Vector2D<float>(w, h));
			marco->addComponent<SpriteRenderer>(marcoTexture, 0, 0);

			totalObjects.push_back(marco);
			totalObjects.push_back(noteObject);
			notes.push_back(noteObject);
		}
		else {
			delete noteObject;
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error creating note %d in Notebook", i);
		}
	}
}

Notebook::~Notebook() {
	for (GameObject* note : totalObjects) {
		delete note;
	}
	notes.clear();
}

void Notebook::revealNote(int index) {

}

void Notebook::render() const {
	for (size_t i = 0; i < totalObjects.size(); i++) {
		if (totalObjects[i]->getIsActive() && totalObjects[i]->spriteRenderer != nullptr && totalObjects[i]->spriteRenderer->isEnabled)
			totalObjects[i]->render();
	}
}

GameObject* Notebook::getNote(int index) const {
	if (index < 0 || index >= notesCount) {
		return nullptr;
	}

	return notes[index];
}