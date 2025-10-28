#include "AudioManager.h"

using namespace std;

struct MusicSpec {
	const char* name;
	bool loop;
	int fade;
};

constexpr const char* const musicBase = "../assets/music/";
constexpr const char* const soundsBase = "../assets/sounds/";

MIX_Mixer* AudioManager::mixer;
MIX_Track* AudioManager::musicTrackA;
MIX_Track* AudioManager::musicTrackB;
MIX_Track* AudioManager::musicTrackC;

bool AudioManager::usingTrackA;
int AudioManager::musicLength;
float AudioManager::elapsedTime;
bool AudioManager::doLoop;

AudioManager::MusicName AudioManager::currentMusic;

int AudioManager::fadeOut = 4;

std::array<AudioManager::AudioData, AudioManager::NUM_MUSIC> AudioManager::music;
std::array<MIX_Audio*, AudioManager::NUM_SOUNDS> AudioManager::sounds;

constexpr array<MusicSpec, AudioManager::NUM_MUSIC> musicList{
	MusicSpec{"main.mp3", 1, 5000},
	{"menu.mp3", 1, 5000},
	{"introP1.mp3", 1, 5000},
	{"introP2.mp3", 1, 5000},
	{"introP3.mp3", 1, 5000},
	{"introP4.mp3", 1, 5000},
	{"riser.mp3", 0, 5000},
	{"finalCerrar.mp3", 1, 5000},
	{"endFinalCerrar.mp3", 0, 5000},
	{"finalP1.mp3", 1, 5000},
	{"finalP2.mp3", 1, 5000},
	{"finalP3.mp3", 1, 5000},
	{"finalP4.mp3", 0, 5000},
	{"discovery.mp3", 0, 5000}
};

constexpr array<const char*, AudioManager::NUM_SOUNDS> soundsList{
	"Page.mp3",
	"Pencil.mp3",
	"Button.wav",
	"uvLight.mp3",
	"uvLightOff.mp3",
	"PencilDraw.mp3",
	"voz1.mp3",
	"voz2.mp3"
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
	musicTrackA = MIX_CreateTrack(mixer);
	musicTrackB = MIX_CreateTrack(mixer);
	musicTrackC = MIX_CreateTrack(mixer);
	if (!musicTrackA || !musicTrackB || !musicTrackC) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error al crear el musicTrack");
	}

	// Cargamos las canciones
	for (size_t i = 0; i < musicList.size(); i++) {
		string name = musicList[i].name;
		bool loop = musicList[i].loop;
		int fade = musicList[i].fade;

		AudioData data;
		data.audio = MIX_LoadAudio(mixer, ((string)musicBase + name).c_str(), true);
		data.prop = SDL_CreateProperties();
		SDL_SetNumberProperty(data.prop, MIX_PROP_PLAY_FADE_IN_MILLISECONDS_NUMBER, fade);

		data.doLoop = loop;

		music[i] = data;
		if (data.audio) SDL_Log("[AudioManager] Pista %i. CORRECTO", i);
	}

	for (size_t i = 0; i < soundsList.size(); i++) {
		string name = string(soundsList[i]);
		sounds[i] = MIX_LoadAudio(mixer, ((string)soundsBase + name).c_str(), true);
		if (sounds[i]) SDL_Log("[AudioManager] Sonido %i. CORRECTO", i);
	}

	usingTrackA = true;
}

void AudioManager::Unload() {
	for (size_t i = 0; i < music.size(); i++) {
		MIX_DestroyAudio(music[i].audio);
	}

	MIX_DestroyTrack(musicTrackA);
	MIX_DestroyMixer(mixer);
	MIX_Quit();
}

void AudioManager::Update(float deltaTime) {
	if (!doLoop) return;

	elapsedTime += deltaTime;

	if (elapsedTime > musicLength - musicDelta) {
		playSong(currentMusic);
		elapsedTime = 0; 
	}
}

void AudioManager::playSong(MusicName name) {
	MIX_Track* track = usingTrackA ? musicTrackA : musicTrackB;
	AudioData& data = music[name];

	doCrossFade(currentMusic, name);

	// Calcular duración del audio en microsegundos
	double durationSamples = MIX_GetAudioDuration(data.audio);
	musicLength = durationSamples / 44100.0;
	doLoop = data.doLoop;
}

void AudioManager::doCrossFade(MusicName audioA, MusicName audioB) {
	MIX_Track* newTrack = usingTrackA ? musicTrackB : musicTrackA;
	AudioData& dataAudioB = music[audioB];

	if (MIX_TrackPlaying(musicTrackA) && MIX_TrackPlaying(musicTrackB)) {

		MIX_SetTrackAudio(musicTrackC, dataAudioB.audio);
		MIX_PlayTrack(musicTrackC, dataAudioB.prop);
		// Fade out del anterior
		stopMusic();
		usingTrackA = true;
		currentMusic = audioB;
	}
	else {
		MIX_SetTrackAudio(newTrack, dataAudioB.audio);
		MIX_PlayTrack(newTrack, dataAudioB.prop);
		// Fade out del anterior
		MIX_StopTrack(usingTrackA ? musicTrackA : musicTrackB, fadeOut * 44100);
		if (MIX_TrackPlaying(musicTrackC)) MIX_StopTrack(musicTrackC, fadeOut * 44100);

		usingTrackA = !usingTrackA;
		currentMusic = audioB;
	}
}

void AudioManager::playSound(SoundName name) {
	MIX_PlayAudio(mixer, sounds[name]);
}

void AudioManager::stopMusic() {
	MIX_StopTrack(musicTrackA, fadeOut * 44100);
	MIX_StopTrack(musicTrackB, fadeOut * 44100);
}