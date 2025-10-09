#include "MonoBehaviour.h"
#include "GameObject.h"
#include <typeinfo> // Para typeid

void MonoBehaviour::Start() {
    SDL_Log("[%s] Iniciado: %s",
        gameObject->getName().c_str(),
        typeid(*this).name());
}