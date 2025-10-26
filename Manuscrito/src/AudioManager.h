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
		INTRO_P1,
		INTRO_P2,
		INTRO_P3,
		INTRO_P4,
		RISER,
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
		bool doLoop;
	};
	
private:
	static MIX_Mixer* mixer;
	static MIX_Track* musicTrackA;
	static MIX_Track* musicTrackB;
	static MIX_Track* musicTrackC; // Solo se utiliza para situaciones donde las otras dos están ocupadas

	static bool usingTrackA;
	static int musicLength;
	// El tiempo que se deja antes de reproducirse a si misma
	static const int musicDelta = 5;
	static float elapsedTime;
	static bool doLoop;

	static int fadeOut;

	static std::array<AudioData, NUM_MUSIC> music;
	static std::array<MIX_Audio*, NUM_SOUNDS> sounds;

public:
	AudioManager() = default;
	~AudioManager() = default;

	static void Init();
	static void Unload();
	static void Update(float deltaTime);
	static void playSong(MusicName name);
	static void playSound(SoundName name);
	static void stopMusic();

	static MusicName currentMusic;

private:
	static void doCrossFade(MusicName audioA, MusicName audioB);
};


