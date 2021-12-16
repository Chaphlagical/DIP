#pragma once

#include <string>

#include <Image.h>

namespace Chaf
{
	class NSFW
	{
	public:
		static std::string check(const cv::Mat& img);
	};
}