#pragma once
#include <SDL3/SDL_mixer.h>
#include <string>
#include <array>
#include <random>

class AudioManager {
public:
	enum MusicName
	{
		MAIN,
		MENU,
		NUM_MUSIC
	};

	enum SoundName
	{
		CHANGE_PAGE,
		PENCIL_CIRCLE,
		BUTTON,
		NUM_SOUNDS
	};

	struct AudioData {
		MIX_Audio* audio;
		SDL_PropertiesID prop;
	};
	
private:
	static MIX_Mixer* mixer;
	static MIX_Track* musicTrackA;
	static MIX_Track* musicTrackB;
	static bool usingTrackA;
	static Uint64 musicLengthUs;  // duración en microsegundos (la API de SDL_mixer usa microsegundos)
	static Uint64 fadeStartTimeUs;
	static bool fadingInProgress;

	static int fadeOut;

	static std::array<AudioData, NUM_MUSIC> music;
	static std::array<MIX_Audio*, NUM_SOUNDS> sounds;

public:
	AudioManager() = default;
	~AudioManager() = default;

	static void Init();
	static void Unload();
	static void Update();
	static void playSong(MusicName name);
	static void playSound(SoundName name);
	static void stopMusic();

	static MusicName currentMusic;

private:
	static void doCrossFade(MusicName audioA, MusicName audioB);
};


