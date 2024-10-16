#pragma once

#include <unordered_map>
#include <memory>
#include <string>

extern "C" {
#include <SDL2/SDL_mixer.h>
}

enum class SoundName {
	GAME_OVER,
	LINE_CLEAR,
	MOVE_PIECE,
	PIECE_LANDED,
	ROCKET_ENDING,
	TETRIS_LINE_CLEAR,
	LEVEL_UP,
	MENU,
	PIECE_FALLING_AFTER_LINE_CLEAR,
	PLAYER_SENDING_BLOCKS,
	ROTATE_PIECE
};

enum class MusicName {
	MAIN_THEME
};

using namespace std;

class Sound {
private:
	static unique_ptr <unordered_map<SoundName, shared_ptr<Mix_Chunk>>> cachedSounds;
	static unique_ptr <unordered_map<MusicName, shared_ptr<Mix_Music>>> cachedMusic;
public:
	Sound( );

	bool PlaySound(SoundName soundName, int loop = 0);
	bool PlayMusic(MusicName musicName, int loop = -1);

	bool PauseMusic( );
	bool ResumeMusic( );
	bool IncreaseVolume( );
	bool DecreaseVolume( );

};
