#include "Notebook.h"
#include "GameObject.h"
#include "Text.h"
#include "texture.h"
#include "NotebookElement.h"
#include "game.h"
#include "NoteRevealer.h"
#include "Button.h"
#include "Scene.h"

#include<SDL3/SDL_ttf.h>

enum Direction {
	UP,
	DOWN,
	LEFT,
	RIGHT
};

Notebook::Notebook(std::istream& is, GameObject* parent, TTF_Font* font, Texture* tex, SDL_Renderer* rend, Scene* scene) {
	notebookObject = parent;
	totalObjects.push_back(notebookObject);

	notesCount = 0;
	discoveredNotes = 0;

	renderer = rend;
	this->font = font;
	marcoTexture = tex;

	std::vector<GameObject*> tempInfoObjects;

	std::string tag;
	std::string title;
	std::string subtitleT;
	//Leemos el titulo
	is >> tag;
	std::getline(is, title);
	is >> tag;
	std::getline(is, subtitleT);
	// Leemos la cantidad de notas
	is >> tag >> notesCount;

	//Creamos el titulo
	GameObject* titleObject = new GameObject("NotebookTitle", 3, notebookObject);
	titleObject->addComponent<Transform>(Vector2D<float>(0, -200), 0.15);
	titleObject->addComponent<SpriteRenderer>();
	Text* titleText = titleObject->addComponent<Text>(title, mainNoteColor, font, 0, TITLE_FONT_SIZE, renderer);

	//Creamos el subtitulo
	GameObject* subtilte = new GameObject("NotebookSubtitle", 3, notebookObject);
	subtilte->addComponent<Transform>(Vector2D<float>(0, -170), 0.15);
	subtilte->addComponent<SpriteRenderer>();
	subtilte->addComponent<Text>(subtitleT, mainNoteColor, font, 0, TITLE_FONT_SIZE - 100, renderer);

	totalObjects.push_back(titleObject);
	totalObjects.push_back(subtilte);
	Game::gameObjects.push_back(titleObject);
	Game::gameObjects.push_back(subtilte);

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
			noteObject->addComponent<NotebookElement>(title, MAIN_FONT_SIZE);
			break;

		case 1: // SECONDARY
			text = noteObject->addComponent<Text>(title, secondaryNoteColor, font, SECONDARY_TEXT_WIDTH, SECONDARY_FONT_SIZE, renderer);
			noteObject->addComponent<NotebookElement>(title, SECONDARY_FONT_SIZE);
			break;

		default:
			break;
		}

		if (text) {
			//Creamos el marco de la nota
			float w = noteObject->spriteRenderer->getTexture()->getWidth() * noteObject->transform->getScale().x + NOTE_SPACING;
			float h = noteObject->spriteRenderer->getTexture()->getHeight() * noteObject->transform->getScale().y + NOTE_SPACING;

			if (h < MIN_NOTE_HEIGHT) h = MIN_NOTE_HEIGHT;

			// Creamos el marco
			GameObject* marco = new GameObject("MarcoNota_" + std::to_string(i), 3, noteObject);
			marco->addComponent<Transform>(Vector2D<float>(0, 0), Vector2D<float>(w, h));
			marco->addComponent<SpriteRenderer>(marcoTexture, 0, 0);
			marco->addComponent<Button>([this, i]() { onClickNote(i); });

			//Creamos la info de la nota
			GameObject* hoverInfo = new GameObject("NoteHoverInfo_" + std::to_string(i), 2, noteObject);
			hoverInfo->addComponent<Transform>(Vector2D<float>(0, -h*2), 0.09);
			hoverInfo->addComponent<SpriteRenderer>(scene->game->getTexture(Game::FOLIO), 0, 0);

			hoverInfo->spriteRenderer->isEnabled = false;

			//Añadimos el texto a la info de la nota
			GameObject* infoTextObject = new GameObject("NoteInfoText_" + std::to_string(i), 1, hoverInfo);
			infoTextObject->addComponent<Transform>(Vector2D<float>(0, 0), 0.15);
			infoTextObject->addComponent<SpriteRenderer>();
			Text* infoText = infoTextObject->addComponent<Text>(desciption, secondaryNoteColor, font, 1100, SECONDARY_FONT_SIZE, renderer);

			infoTextObject->spriteRenderer->isEnabled = false;

			// Asignamos las conexiones
			noteConnections[i] = relatedNotes;

			// Guardamos los objetos a renderizar y la nota
			totalObjects.push_back(marco);
			totalObjects.push_back(noteObject);
			notes.push_back(noteObject);
			infoObjects.push_back(hoverInfo);
			tempInfoObjects.push_back(infoTextObject);

			noteObject->spriteRenderer->isEnabled = false;
			marco->spriteRenderer->isEnabled = false;

			Game::gameObjects.push_back(noteObject);
			Game::gameObjects.push_back(marco);
			Game::gameObjects.push_back(hoverInfo);
			Game::gameObjects.push_back(infoTextObject);

			scene->sceneObjects.push_back(marco);
		}
		else {
			delete noteObject;
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error creating note %d in Notebook", i);
		}
	}

	// Se añade despues para que este por encima
	for (int i = 0; i < infoObjects.size(); i++) {
		totalObjects.push_back(infoObjects[i]);
		totalObjects.push_back(tempInfoObjects[i]);
	}
}

Notebook::~Notebook() {
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

void Notebook::update(float deltaTime) {
	
}

void Notebook::render() const {
	//renderizamos el fondo
	totalObjects[0]->render(); 

	//renderizamos las lineas de conexion
	renderLines();

	//renderizamos las notas
	for (size_t i = 1; i < totalObjects.size(); i++) {
		if (totalObjects[i]->getIsActive() && totalObjects[i]->spriteRenderer != nullptr && totalObjects[i]->spriteRenderer->isEnabled)
			totalObjects[i]->render();
	}
}

void Notebook::renderLines() const {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Color dorado para las líneas

	for (int i = 0; i < noteConnections.size(); i++) {

		if (noteConnections.contains(i) && notes[i]->spriteRenderer->IsActive()) {
			int connectionsCount = noteConnections.at(i).size();
			GameObject* noteA = notes[i];
			NotebookElement* neA = noteA->getComponent<NotebookElement>();

			if (neA) {
				if (neA->discovered == false) {
					continue; // Si la nota A no está descubierta, no dibujamos sus conexiones
				}
			}
			else {
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error retrieving NotebookElement component for note %d", i);
				continue;
			}

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

void Notebook::onClickNote(int index) {
	if (index < 0 || index >= notesCount) {
		return;
	}

	if (notes[index]->getComponent<NotebookElement>()->discovered == false) {
		return;
	}

	if (lastInfoObject && lastInfoObject != infoObjects[index]) {
		lastInfoObject->spriteRenderer->isEnabled = false;
		if (lastInfoObject->getChild(0)) {
			lastInfoObject->getChild(0)->spriteRenderer->isEnabled = false;
		}
	}

	infoObjects[index]->spriteRenderer->isEnabled = !infoObjects[index]->spriteRenderer->isEnabled;

	if (infoObjects[index]->getChild(0)) {
		infoObjects[index]->getChild(0)->spriteRenderer->isEnabled = infoObjects[index]->spriteRenderer->isEnabled;
	}
	lastInfoObject = infoObjects[index];
}