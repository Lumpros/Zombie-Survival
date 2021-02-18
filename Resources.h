#pragma once

#include <unordered_map>
#include <string>
#include <memory>

#include "Text.h"
#include "Texture.h"
#include "Sound.h"

namespace PZS
{
	/* Holds Texture, Font, Text... etc in an unordered map paired with a string */
	template <typename Resource> class ResourceHolder
	{
	private:
		std::unordered_map<std::string, std::unique_ptr<Resource>> resources;

	public:
		const std::unordered_map<std::string, std::unique_ptr<Resource>>* GetContainerCopy(void) const noexcept
		{
			return &resources;
		}

		void Add(const std::string& name, Resource* resource) noexcept
		{
			if (resources.find(name) == resources.end())
			{
				resources.insert(std::make_pair(name, resource));
			}
		}

		Resource* Get(const std::string& name) noexcept
		{
			auto find_resource = resources.find(name);

			if (find_resource != resources.end())
			{
				return find_resource->second.get();
			}

			return nullptr;
		}

		void Destroy(void) noexcept 
		{
			resources.clear();
		}
	};

	/* Pretty straight forward. Just a way to access all the resources held in the ResourceHolder class */
	class Resources
	{
	private:
		Resources(void) noexcept;

	public:
		Resources(const Resources&) = delete;
		void operator=(const Resources&) = delete;

		static Resources* GetInstance(void) noexcept;

		void SetMixChunkVolume(int volume) noexcept;
		void Destroy(void) noexcept;

	public:
		ResourceHolder<Texture> gTextures;
		ResourceHolder<Font>    gFonts;
		ResourceHolder<SFX>     gSFX;
		ResourceHolder<Music>   gMusic;
	};
}

