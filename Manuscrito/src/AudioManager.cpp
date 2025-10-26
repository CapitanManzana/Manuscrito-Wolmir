#include "AudioManager.h"

using namespace std;

struct MusicSpec {
	const char* name;
	int loop;
	int fade;
};

constexpr const char* const musciBase = "../assets/music/";

constexpr array<MusicSpec, AudioManager::NUM_MUSIC> musicList{
	MusicSpec{"The River.mp3", -1, 5000}
};

AudioManager::AudioManager() {
	// Inicializa SDL_mixer (puedes especificar formatos como OGG, MP3)
	if (!MIX_Init()) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No se ha cargado SDL_mixer");
	}

	//Creamos el mixer
	mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
	if (!mixer) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error al crear el mixer");
	}

	// Creamos el track para la musica
	musicTrack = MIX_CreateTrack(mixer);
	if (!musicTrack) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error al crear el musicTrack");
	}

	for (size_t i = 0; i < musicList.size(); i++) {
		string name = musicList[i].name;
		int loop = musicList[i].loop;
		int fade = musicList[i].fade;

		MusicData data;
		data.music = MIX_LoadAudio(mixer, ((string)musciBase + name).c_str(), true);
		data.prop = SDL_CreateProperties();
		SDL_SetNumberProperty(data.prop, MIX_PROP_PLAY_LOOPS_NUMBER, loop);
		SDL_SetNumberProperty(data.prop, MIX_PROP_PLAY_FADE_IN_MILLISECONDS_NUMBER, fade);

		music[i] = data;
		SDL_Log("[AudioManager] %s con valores loop: %i y fade: %i. CORRECTO", name, loop, fade);
	}
}

AudioManager::~AudioManager() {
	for (size_t i = 0; i < music.size(); i++) {
		MIX_DestroyAudio(music[i].music);
	}

	MIX_DestroyTrack(musicTrack);
	MIX_DestroyMixer(mixer);
	MIX_Quit();
}

void AudioManager::playSong(MusicName name) {
	MIX_SetTrackAudio(musicTrack, music[name].music);
	MIX_PlayTrack(musicTrack, music[name].prop);
}

void AudioManager::stopMusic() {
	MIX_StopTrack(musicTrack, fadeOutMiliseconds);
}