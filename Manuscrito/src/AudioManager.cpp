#include "AudioManager.h"

using namespace std;

struct MusicSpec {
	const char* name;
	int loop;
	int fade;
};

constexpr const char* const musicBase = "../assets/music/";
constexpr const char* const soundsBase = "../assets/sounds/";

MIX_Mixer* AudioManager::mixer;
MIX_Track* AudioManager::musicTrack;
MIX_Track* AudioManager::soundsTrack;

int AudioManager::fadeOutMiliseconds = 100000;

std::array<AudioManager::AudioData, AudioManager::NUM_MUSIC> AudioManager::music;
std::array<MIX_Audio*, AudioManager::NUM_SOUNDS> AudioManager::sounds;

constexpr array<MusicSpec, AudioManager::NUM_MUSIC> musicList{
	MusicSpec{"The River.mp3", -1, 5000}
};

constexpr array<const char*, AudioManager::NUM_SOUNDS> soundsList{
	"Page.mp3",
	"Pencil.mp3",
	"Button.wav"
};

void AudioManager::Init() {
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

	// Cargamos las canciones
	for (size_t i = 0; i < musicList.size(); i++) {
		string name = musicList[i].name;
		int loop = musicList[i].loop;
		int fade = musicList[i].fade;

		AudioData data;
		data.audio = MIX_LoadAudio(mixer, ((string)musicBase + name).c_str(), true);
		data.prop = SDL_CreateProperties();
		SDL_SetNumberProperty(data.prop, MIX_PROP_PLAY_LOOPS_NUMBER, loop);
		SDL_SetNumberProperty(data.prop, MIX_PROP_PLAY_FADE_IN_MILLISECONDS_NUMBER, fade);

		music[i] = data;
		if (data.audio) SDL_Log("[AudioManager] Pista %i. CORRECTO", i);
	}

	for (size_t i = 0; i < soundsList.size(); i++) {
		string name = string(soundsList[i]);
		sounds[i] = MIX_LoadAudio(mixer, ((string)soundsBase + name).c_str(), true);
		if (sounds[i]) SDL_Log("[AudioManager] Sonido %i. CORRECTO", i);
	}
}

void AudioManager::Unload() {
	for (size_t i = 0; i < music.size(); i++) {
		MIX_DestroyAudio(music[i].audio);
	}

	MIX_DestroyTrack(musicTrack);
	MIX_DestroyMixer(mixer);
	MIX_Quit();
}

void AudioManager::playSong(MusicName name) {
	MIX_SetTrackAudio(musicTrack, music[name].audio);
	MIX_PlayTrack(musicTrack, music[name].prop);
}

void AudioManager::playSound(SoundName name) {
	MIX_PlayAudio(mixer, sounds[name]);
}

void AudioManager::stopMusic() {
	MIX_StopTrack(musicTrack, fadeOutMiliseconds);
}