#pragma once

#include <SDL.h>
#include <vector>
#include <ctime>

namespace PZS
{
	struct Frame
	{
		/* Clip is the part of the image used in the frame */
		SDL_Rect clip;
		int duration_milliseconds;
	};

	/* A basic class used for animations (either looped or played once */
	/* Keeps no information regarding the texture, just calculates the */
	/* rectangle that should be used in the render() call */
	class Animation
	{
	private:
		std::vector<Frame> frames;
		bool animation_is_running = true;

		/* current index in the frames vector. Used to decide whether the animation should restart */
		unsigned int current_index = 0;

		clock_t start;

	public:
		void AddFrame(Frame frame);
		SDL_Rect* GetCurrentFrame(void);
		unsigned int GetFrameIndex(void) const;

		void Reset(void);
		void Update(bool playOnce = false);
		void Pause(bool Pause);

		bool IsRunning(void) const;

		Animation(void);
	};
}