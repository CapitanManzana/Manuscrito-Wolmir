#pragma once
#include <SDL3/SDL_mixer.h>
#include <string>
#include <array>
#include <random>

class AudioManager {
public:
	enum MusicName
	{
		THE_RIVER,
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
	static MIX_Track* musicTrack;
	static MIX_Track* soundsTrack;

	static int fadeOutMiliseconds;

	static std::array<AudioData, NUM_MUSIC> music;
	static std::array<MIX_Audio*, NUM_SOUNDS> sounds;

public:
	AudioManager() = default;
	~AudioManager() = default;

	static void Init();
	static void Unload();
	static void playSong(MusicName name);
	static void playSound(SoundName name);
	static void stopMusic();
};


