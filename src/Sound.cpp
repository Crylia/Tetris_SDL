#include "Sound.hpp"

unique_ptr<unordered_map<SoundName, shared_ptr<Mix_Chunk>>> Sound::cachedSounds = nullptr;
unique_ptr<unordered_map<MusicName, shared_ptr<Mix_Music>>> Sound::cachedMusic = nullptr;

Sound::Sound( ) {
	if (!cachedSounds) {
		cachedSounds = make_unique<unordered_map<SoundName, shared_ptr<Mix_Chunk>>>( );

		cachedSounds->emplace(
			SoundName::GAME_OVER,
			std::shared_ptr<Mix_Chunk>(Mix_LoadWAV("assets/sound_effects/game_over.wav"), [ ](Mix_Chunk* r) { Mix_FreeChunk(r); })
		);
		cachedSounds->emplace(
			SoundName::LINE_CLEAR,
			std::shared_ptr<Mix_Chunk>(Mix_LoadWAV("assets/sound_effects/line_clear.wav"), [ ](Mix_Chunk* r) { Mix_FreeChunk(r); })
		);
		cachedSounds->emplace(
			SoundName::MOVE_PIECE,
			std::shared_ptr<Mix_Chunk>(Mix_LoadWAV("assets/sound_effects/move_piece.wav"), [ ](Mix_Chunk* r) { Mix_FreeChunk(r); })
		);
		cachedSounds->emplace(
			SoundName::PIECE_LANDED,
			std::shared_ptr<Mix_Chunk>(Mix_LoadWAV("assets/sound_effects/piece_landed.wav"), [ ](Mix_Chunk* r) { Mix_FreeChunk(r); })
		);
		cachedSounds->emplace(
			SoundName::ROCKET_ENDING,
			std::shared_ptr<Mix_Chunk>(Mix_LoadWAV("assets/sound_effects/rocket_ending.wav"), [ ](Mix_Chunk* r) { Mix_FreeChunk(r); })
		);
		cachedSounds->emplace(
			SoundName::TETRIS_LINE_CLEAR,
			std::shared_ptr<Mix_Chunk>(Mix_LoadWAV("assets/sound_effects/tetris_line_clear.wav"), [ ](Mix_Chunk* r) { Mix_FreeChunk(r); })
		);
		cachedSounds->emplace(
			SoundName::LEVEL_UP,
			std::shared_ptr<Mix_Chunk>(Mix_LoadWAV("assets/sound_effects/level_up.wav"), [ ](Mix_Chunk* r) { Mix_FreeChunk(r); })
		);
		cachedSounds->emplace(
			SoundName::MENU,
			std::shared_ptr<Mix_Chunk>(Mix_LoadWAV("assets/sound_effects/menu.wav"), [ ](Mix_Chunk* r) { Mix_FreeChunk(r); })
		);
		cachedSounds->emplace(
			SoundName::PIECE_FALLING_AFTER_LINE_CLEAR,
			std::shared_ptr<Mix_Chunk>(Mix_LoadWAV("assets/sound_effects/piece_falling_after_line_clear.wav"), [ ](Mix_Chunk* r) { Mix_FreeChunk(r); })
		);
		cachedSounds->emplace(
			SoundName::PLAYER_SENDING_BLOCKS,
			std::shared_ptr<Mix_Chunk>(Mix_LoadWAV("assets/sound_effects/player_sending_blocks.wav"), [ ](Mix_Chunk* r) { Mix_FreeChunk(r); })
		);
		cachedSounds->emplace(
			SoundName::ROTATE_PIECE,
			std::shared_ptr<Mix_Chunk>(Mix_LoadWAV("assets/sound_effects/rotate_piece.wav"), [ ](Mix_Chunk* r) { Mix_FreeChunk(r); })
		);
	}
	if (!cachedMusic) {
		cachedMusic = make_unique<unordered_map<MusicName, shared_ptr<Mix_Music>>>( );
		cachedMusic->emplace(
			MusicName::MAIN_THEME,
			shared_ptr<Mix_Music>(Mix_LoadMUS("assets/sound_tracks/bgm.mp3"), [ ](Mix_Music* r) {Mix_FreeMusic(r);})
		);
	}
}

bool Sound::PlaySound(SoundName soundName, int loop) {
	auto it = cachedSounds->find(soundName);
	if (it == cachedSounds->end( ) || !it->second) {
		return false;
	}

	Mix_PlayChannel(-1, it->second.get( ), loop);

	return true;
}

bool Sound::PlayMusic(MusicName musicName, int loop) {
	auto it = cachedMusic->find(musicName);
	if (it == cachedMusic->end( ) || !it->second) {
		return false;
	}

	Mix_PlayMusic(it->second.get( ), loop);

	return true;
}

void Sound::PauseMusic( ) {
	if (Mix_PlayingMusic( ) != 0)
		Mix_PauseMusic( );
}

void Sound::ResumeMusic( ) {
	if (Mix_PausedMusic( ))
		Mix_ResumeMusic( );
}

void Sound::IncreaseVolume( ) {
	int currentVolume = Mix_Volume(-1, -1);
	if (currentVolume < MIX_MAX_VOLUME)
		Mix_Volume(-1, currentVolume + 2);
}

void Sound::DecreaseVolume( ) {
	int currentVolume = Mix_Volume(-1, -1);
	if (currentVolume > 0)
		Mix_Volume(-1, currentVolume - 2);
}

bool Sound::IsMusicPlaying( ) { return Mix_PlayingMusic( ) && !Mix_PausedMusic( ); }
