#pragma once

#include <Base.h>

#include <Image.h>

namespace Chaf
{
	class Downsampling
	{
	public:
		enum class Method
		{
			Nearest,
			Linear,
			Cubic,
			Lanczos,
			Pyramid
		};

		static void downsampling(Ref<Image>& src, Ref<Image>& dst, Method method);
	};
}