#include "Animation.h"

void PZS::Animation::AddFrame(PZS::Frame frame)
{
	frames.push_back({ frame.clip, frame.duration_milliseconds });
}

SDL_Rect* PZS::Animation::GetCurrentFrame(void)
{
	return &frames[current_index].clip;
}

unsigned int PZS::Animation::GetFrameIndex(void) const
{
	return current_index;
}

void PZS::Animation::Reset(void)
{
	current_index = 0;
}

bool PZS::Animation::IsRunning(void) const
{
	return animation_is_running;
}

PZS::Animation::Animation(void)
{
	start = clock();
}

void PZS::Animation::Update(bool playOnce)
{
	if (animation_is_running)
	{
		if (clock() - start >= frames[current_index].duration_milliseconds)
		{
			++current_index;

			if (current_index == frames.size())
			{
				current_index = 0;

				/* No reason to repeat */
				if (playOnce)
				{
					animation_is_running = false;
				}
			}

			start = clock();
		}
	}
}

void PZS::Animation::Pause(bool Pause)
{
	animation_is_running = !Pause;
}