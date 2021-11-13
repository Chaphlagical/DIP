#pragma once

#include <Image.h>

namespace Chaf
{
	class Similarity
	{
	public:
		static float SSIM(Image& img1, Image& img2);
		static float PSNR(Image& img1, Image& img2);
	};
}