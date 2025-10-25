#include "IntroScene.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "Text.h"

using namespace std;

void IntroScene::Load() {
	GameObject* texto = new GameObject("Texto", 3);
	string s = "Febrero de 1972, eres un historiador doctorando en manuscritos antiguos. En plena noche recibes una llamada, es Franz Wolmir, un multimillonario alemán, te necesita urgentemente para que desficres un extraño manuscrito que lleva en su familia muchas generaciones, es el conocido Manuscrito Wolmir.";
	SDL_Color color = { 255, 255, 255, 255 };
	
	// Añadimos los componentes
	texto->addComponent<Transform>(Vector2D<float>(Game::WINDOW_WIDTH / 2, Game::WINDOW_HEIGHT / 2), 0.15);
	texto->addComponent<SpriteRenderer>();
	texto->addComponent<Text>(s, color, game->baseFontCentered, 4000, 100, renderer);

	game->gameObjects.push_back(texto);
	sceneObjects.push_back(texto);
}

void IntroScene::HandleEvents(SDL_Event& event) {
	if (event.type == SDL_EVENT_KEY_DOWN) {
		if (event.key.key == SDLK_T) {
			SceneManager::changeScene(1);
		}
	}
}