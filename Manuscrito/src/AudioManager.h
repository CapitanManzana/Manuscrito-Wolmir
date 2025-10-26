#pragma once
#include <SDL3/SDL_mixer.h>
#include <string>
#include <array>

class AudioManager {
public:
	enum MusicName
	{
		THE_RIVER,
		NUM_MUSIC
	};

	struct MusicData {
		MIX_Audio* music;
		SDL_PropertiesID prop;
	};

	std::array<MusicData, NUM_MUSIC> music;

private:
	MIX_Mixer* mixer;
	MIX_Track* musicTrack;

	int fadeOutMiliseconds = 50000;

public:
	AudioManager();
	~AudioManager();

	void playSong(MusicName name);
	void stopMusic();
};


