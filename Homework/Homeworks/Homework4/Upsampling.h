#pragma once

#include <Base.h>

#include <Image.h>

namespace Chaf
{
	class Upsampling
	{
	public:
		enum class Method
		{
			Nearest,
			Linear,
			Cubic,
			Lanczos,
			Pyramid,
			EDSR,
			ESPCN,
			FSRCNN,
			LapSRN
		};

		static void upsampling(Ref<Image>& src, Ref<Image>& dst, Method method);
	};
}