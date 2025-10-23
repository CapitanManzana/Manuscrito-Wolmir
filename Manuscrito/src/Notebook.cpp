#include "Notebook.h"
#include "GameObject.h"
#include "Text.h"
#include "texture.h"
#include "NotebookElement.h"
#include "game.h"
#include "NoteRevealer.h"

#include<SDL3_ttf/SDL_ttf.h>

enum Direction {
	UP,
	DOWN,
	LEFT,
	RIGHT
};

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

		//Leemos esta estructura
		/*
		* Title: 13 de Octubre de 1534
		* Description: ""
		* PosX: 0
		* PosY: 0
		* Category: 0
		* RevealTextIndex: 0
		* Conexions: 1
		*	- 1
		*/
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

		// Creamos el objeto nota que es el texto
		GameObject* noteObject = new GameObject("Note_" + std::to_string(i), 5, notebookObject);
		noteObject->addComponent<Transform>(position, 0.15);
		noteObject->addComponent<SpriteRenderer>();
		Text* text = nullptr;

		// Dependiendo de la importancia de la nota, le damos un formato u otro
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
			noteObject->addComponent<NotebookElement>(title);

			//Creamos el marco de la nota
			float w = noteObject->spriteRenderer->getTexture()->getWidth() * noteObject->transform->getScale().x + NOTE_SPACING;
			float h = noteObject->spriteRenderer->getTexture()->getHeight() * noteObject->transform->getScale().y + NOTE_SPACING;

			if (h < MIN_NOTE_HEIGHT) h = MIN_NOTE_HEIGHT;

			GameObject* marco = new GameObject("MarcoNota_" + std::to_string(i), 2, noteObject);
			marco->addComponent<Transform>(Vector2D<float>(0, 0), Vector2D<float>(w, h));
			marco->addComponent<SpriteRenderer>(marcoTexture, 0, 0);

			// Asignamos las conexiones
			noteConnections[i] = relatedNotes;

			// Guardamos los objetos a renderizar y la nota
			totalObjects.push_back(marco);
			totalObjects.push_back(noteObject);
			notes.push_back(noteObject);

			noteObject->spriteRenderer->isEnabled = true;
			marco->spriteRenderer->isEnabled = true;
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

void Notebook::discoverNote(int index) {
	if (index < 0 || index >= notesCount) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Trying to reveal invalid note index %d", index);
		return;
	}

	NotebookElement* ne = notes[index]->getComponent<NotebookElement>();
	if (ne) {
		ne->discovered = true;
		ne->reveal();

		std::vector<int> connections = noteConnections[index];
		if (connections.size() > 0) {

			//Recorremos el vector de notas conectadas y las activamos
			for (int connIndex : connections) {
				NotebookElement* ne = notes[connIndex]->getComponent<NotebookElement>();
				if (ne) {
					ne->reveal();
				}
			}
		}
	}
}

void Notebook::render() const {
	for (size_t i = 0; i < totalObjects.size(); i++) {
		if (totalObjects[i]->getIsActive() && totalObjects[i]->spriteRenderer != nullptr && totalObjects[i]->spriteRenderer->isEnabled)
			totalObjects[i]->render();
	}

	renderLines();
}

void Notebook::renderLines() const {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Color dorado para las líneas

	for (int i = 0; i < notesCount; i++) {

		if (noteConnections.contains(i) && notes[i]->spriteRenderer->IsActive()) {
			int connectionsCount = noteConnections.at(i).size();
			GameObject* noteA = notes[i];

			for (int j = 0; j < connectionsCount; j++) {
				GameObject* noteB = notes[noteConnections.at(i)[j]];
				if (!noteB) {
					SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error retrieving connected note %d for note %d", noteConnections.at(i)[j], i);
					continue;
				}

				if (noteB->spriteRenderer->IsActive()) {

					// Calculamos el centro de ambas notas
					Transform* transformA = noteA->transform;
					Transform* transformB = noteB->transform;
					SpriteRenderer* spriteA = noteA->spriteRenderer;
					SpriteRenderer* spriteB = noteB->spriteRenderer;

					if (!transformA || !transformB || !spriteA || !spriteB) {
						SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error calculating note connection line centers.");
						continue;
					}

					// Calculamos los centros
					float centerA_x = transformA->getGlobalPosition().x;
					float centerA_y = transformA->getGlobalPosition().y;

					float centerB_x = transformB->getGlobalPosition().x;
					float centerB_y = transformB->getGlobalPosition().y;

					// Vector desde A hasta B
					float dx = centerB_x - centerA_x;
					float dy = centerB_y - centerA_y;

					// Tamaños de las notas
					float halfWidthA = ((spriteA->getTexture()->getWidth() + NOTE_SPACING) * transformA->getScale().x) / 2;
					float halfHeightA = ((spriteA->getTexture()->getHeight() + NOTE_SPACING) * transformA->getScale().y) / 2;

					float halfWidthB = ((spriteB->getTexture()->getWidth() + NOTE_SPACING) * transformB->getScale().x) / 2;
					float halfHeightB = ((spriteB->getTexture()->getHeight() + NOTE_SPACING) * transformB->getScale().y) / 2;

					// Normalizamos el vector
					float len = sqrt(dx * dx + dy * dy);
					float nx = dx / len;
					float ny = dy / len;

					// Desde el borde de A hacia afuera
					float posA_x = centerA_x + nx * halfWidthA;
					float posA_y = centerA_y + ny * halfHeightA;

					// Desde el borde de B hacia adentro
					float posB_x = centerB_x - nx * halfWidthB;
					float posB_y = centerB_y - ny * halfHeightB;

					// Dibujamos la línea entre los centros
					SDL_RenderLine(renderer, posA_x, posA_y, posB_x, posB_y);
				}
			}
		}
	}
}


GameObject* Notebook::getNote(int index) const {
	if (index < 0 || index >= notesCount) {
		return nullptr;
	}

	return notes[index];
}

Direction nodeDirection(Vector2D<float> A, Vector2D<float> B) {
	if (A.x < B.x) {
		return RIGHT;
	}
	else if (A.x > B.x) {
		return LEFT;
	}
	else if (A.y < B.y) {
		return DOWN;
	}
	else {
		return UP;
	}
}