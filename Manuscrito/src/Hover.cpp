#include "Hover.h"
#include "GameObject.h"

void Hover::Update(float deltaTime) {
    bool currentlyHovering = checkCursorOver();

    if (currentlyHovering) {
        if (!isHovering && onEnterHover) {
            onEnterHover(); // se ejecuta solo una vez al entrar
        }

        if (onStayHover) {
            onStayHover(); // se ejecuta constantemente mientras está encima
        }
    }
    else {
        if (isHovering && onExitHover) {
            onExitHover(); // se ejecuta solo una vez al salir
        }
    }

    isHovering = currentlyHovering;
}

bool Hover::checkCursorOver() {
    if (!renderer) return false;

    float mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    float logicalX, logicalY;

    SDL_RenderCoordinatesFromWindow(renderer, mouseX, mouseY, &logicalX, &logicalY);

    SDL_FRect rect = gameObject->transform->dstRect;
    return logicalX >= rect.x && logicalX <= rect.x + rect.w &&
        logicalY >= rect.y && logicalY <= rect.y + rect.h;
}
