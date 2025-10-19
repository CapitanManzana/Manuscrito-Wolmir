#include "MonoBehaviour.h"
#include "GameObject.h"
#include <typeinfo> // Para typeid

void MonoBehaviour::Start() {
    if (gameObject) {
        SDL_Log("[%s] Iniciado: %s", gameObject->getName().c_str(), typeid(*this).name());
    }
    else {
        SDL_Log("[%s] Iniciado", typeid(*this).name());
    }

    started = true;
}