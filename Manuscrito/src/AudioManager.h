#pragma once
#include <SDL3/SDL_mixer.h>
#include <string>
#include <unordered_map>

class AudioManager {
public:
    static AudioManager& getInstance();

    bool init();
    void close();

    // Cargar sonidos y música
    void loadSound(const std::string& id, const std::string& path);
    void loadMusic(const std::string& id, const std::string& path);

    // Reproducir
    void playSound(const std::string& id, int loops = 0);
    void playMusic(const std::string& id, int loops = -1);

    // Control de volumen
    void setSoundVolume(const std::string& id, int volume);
    void setMusicVolume(int volume);

private:
    AudioManager() = default;
    ~AudioManager() = default;
};


