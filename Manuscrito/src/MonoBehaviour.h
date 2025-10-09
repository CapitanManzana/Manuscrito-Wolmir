#pragma once
#include <SDL3/SDL.h>
#include "GameObject.h"

class MonoBehaviour {
public:
	GameObject* gameObject = nullptr; // Puntero al GameObject asociado

    MonoBehaviour() : started(false), active(true) {}
    virtual ~MonoBehaviour() = default;

    /// Llamada una sola vez al principio
    virtual void Start() {}

    /// Llamada cada frame
    virtual void Update(float deltaTime) {}

    /// Llamada cuando el objeto se destruye
    virtual void OnDestroy() {}

    /// Método de control general
    void DoUpdate(float deltaTime) {
        if (!started) {
            Start();
            started = true;
        }
        if (active)
            Update(deltaTime);
    }

    bool IsActive() const { return active; }
    void SetActive(bool value) { active = value; }

protected:
    bool started;
    bool active;
};