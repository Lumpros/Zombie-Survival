#pragma once

#include "Entity.h"

namespace PZS
{
	class Barrier : public Entity
	{
	private:

	public:
		explicit Barrier(SDL_Rect rect) noexcept;

		void Update(void) noexcept override;
		void Render(void) noexcept override;
	};
}

