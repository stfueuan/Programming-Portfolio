#include <graphics/texture.h>
#include <graphics/image_data.h>
#include <system/platform.h>

namespace abfw
{
	Texture::Texture()
	{
	}

	Texture::Texture(const class Platform& platform, const ImageData& image_data)
	{
	}

	Texture::~Texture()
	{
	}

	Texture* Texture::CreateCheckerTexture(const Int32 size, const Int32 num_checkers, const Platform& platform)
	{
		const UInt32 check_size = size / num_checkers;
		UInt32* checker_texture = new UInt32[size*size];

		for(UInt32 y = 0; y < size; ++y)
			for(UInt32 x = 0; x < size; ++x)
			{
				if(((y / check_size) % 2) == 0)
				{
					if(((x / check_size) % 2) == 0)
						checker_texture[y*size+x] = 0xffffffff;
					else
						checker_texture[y*size+x] = 0xff000000;
				}
				else
				{
					if(((x / check_size) % 2) == 0)
						checker_texture[y*size+x] = 0xff000000;
					else
						checker_texture[y*size+x] = 0xffffffff;
				}
			}

		ImageData image_data;
		image_data.set_image(reinterpret_cast<UInt8*>(checker_texture));
		image_data.set_width(size);
		image_data.set_height(size);
		Texture* texture = platform.CreateTexture(image_data);

		// checker_texture will get freed up when
		// image_data goes out of scope
		return texture;
	}
}