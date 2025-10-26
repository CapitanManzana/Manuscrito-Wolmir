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
MIX_Track* AudioManager::musicTrackA;
MIX_Track* AudioManager::musicTrackB;

bool AudioManager::usingTrackA;
Uint64 AudioManager::musicLengthUs;  // duración en microsegundos (la API de SDL_mixer usa microsegundos)
Uint64 AudioManager::fadeStartTimeUs;
bool AudioManager::fadingInProgress;

AudioManager::MusicName AudioManager::currentMusic;

int AudioManager::fadeOut = 5;

std::array<AudioManager::AudioData, AudioManager::NUM_MUSIC> AudioManager::music;
std::array<MIX_Audio*, AudioManager::NUM_SOUNDS> AudioManager::sounds;

constexpr array<MusicSpec, AudioManager::NUM_MUSIC> musicList{
	MusicSpec{"main.mp3", 0, 7000},
	{"menu.mp3", -1, 7000}
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
	musicTrackA = MIX_CreateTrack(mixer);
	musicTrackB = MIX_CreateTrack(mixer);
	if (!musicTrackA || !musicTrackB) {
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

	usingTrackA = true;
	fadingInProgress = false;
	musicLengthUs = 0;
}

void AudioManager::Unload() {
	for (size_t i = 0; i < music.size(); i++) {
		MIX_DestroyAudio(music[i].audio);
	}

	MIX_DestroyTrack(musicTrackA);
	MIX_DestroyMixer(mixer);
	MIX_Quit();
}

void AudioManager::Update() {
	if (musicLengthUs == 0 || fadingInProgress) return;

	Uint64 now = SDL_GetTicksNS() / 1000ULL; // tiempo actual en microsegundos
	Uint64 elapsed = now - fadeStartTimeUs;

	// Empieza nueva pista 5 segundos (5e6 µs) antes de que acabe
	if (elapsed > (musicLengthUs - 5'000'000)) {
		fadingInProgress = true;

		doCrossFade(currentMusic, currentMusic);

		usingTrackA = !usingTrackA;
		fadeStartTimeUs = now;
		fadingInProgress = false;
	}
}

void AudioManager::playSong(MusicName name) {
	MIX_Track* track = usingTrackA ? musicTrackA : musicTrackB;
	AudioData& data = music[name];

	if (MIX_TrackPlaying(track)) {
		doCrossFade(currentMusic, name);
	}
	else {
		currentMusic = name;

		MIX_SetTrackAudio(musicTrackA, data.audio);
		MIX_PlayTrack(musicTrackA, data.prop);
	}

	// Calcular duración del audio en microsegundos
	double durationSamples = MIX_GetAudioDuration(data.audio);
	double durationSeconds = durationSamples / 44100.0;
	musicLengthUs = static_cast<Uint64>(durationSeconds * 1'000'000);

	fadeStartTimeUs = SDL_GetTicksNS() / 1000ULL;
	fadingInProgress = false;
}

void AudioManager::doCrossFade(MusicName audioA, MusicName audioB) {
	MIX_Track* newTrack = usingTrackA ? musicTrackB : musicTrackA;
	AudioData& dataAudioB = music[audioB];

	MIX_SetTrackAudio(newTrack, dataAudioB.audio);
	MIX_PlayTrack(newTrack, dataAudioB.prop);
	// Fade out del anterior
	MIX_StopTrack(usingTrackA ? musicTrackA : musicTrackB, fadeOut * 44100);

	usingTrackA = !usingTrackA;
	currentMusic = audioB;
}

void AudioManager::playSound(SoundName name) {
	MIX_PlayAudio(mixer, sounds[name]);
}

void AudioManager::stopMusic() {
	MIX_StopTrack(musicTrackA, fadeOut);
}