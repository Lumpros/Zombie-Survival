#pragma once

#include <SDL_mixer.h>

namespace PZS
{
	class Music
	{
	private:
		Mix_Music* m_Music = nullptr;

	public:
		explicit Music(const char* music_path) noexcept;
		~Music(void) noexcept;

		bool LoadFromFile(const char* music_path) noexcept;

		/* Play(int)
		* 
		* int loop: how many times we want to repeat the music
		*			if loop == -1, then the music is looped until
		*			explicitly stopped
		*/
		void Play(int loop);
		void Stop(void);

		/* Clarity */
		inline static bool IsMusicPlaying(void) noexcept
		{
			return Mix_PlayingMusic();
		}

		/* Clarity */
		inline static bool IsMusicPaused(void) noexcept
		{
			return Mix_PausedMusic();
		}
	};

	class SFX
	{
	private:
		Mix_Chunk* m_Chunk = nullptr;

	public:
		explicit SFX(const char* sfx_path) noexcept;
		~SFX(void) noexcept;

		bool LoadFromFile(const char* sfx_path) noexcept;

		void Play(void);
	};
}