#include "Downsampling.h"

namespace Chaf
{
	void Downsampling::downsampling(Ref<Image>& src, Ref<Image>& dst, Method method)
	{
		switch (method)
		{
		case Method::Nearest:
			cv::resize(src->getImage(), dst->getImage(), { (static_cast<int>(src->getWidth()) + 1) / 2, (static_cast<int>(src->getHeight()) + 1) / 2 }, 0.0, 0.0, cv::INTER_NEAREST);
			break;
		case Method::Linear:
			cv::resize(src->getImage(), dst->getImage(), { (static_cast<int>(src->getWidth()) + 1) / 2, (static_cast<int>(src->getHeight()) + 1) / 2 }, 0.0, 0.0, cv::INTER_LINEAR);
			break;
		case Method::Cubic:
			cv::resize(src->getImage(), dst->getImage(), { (static_cast<int>(src->getWidth()) + 1) / 2, (static_cast<int>(src->getHeight()) + 1) / 2 }, 0.0, 0.0, cv::INTER_CUBIC);
			break;
		case Method::Lanczos:
			cv::resize(src->getImage(), dst->getImage(), { (static_cast<int>(src->getWidth()) + 1) / 2, (static_cast<int>(src->getHeight()) + 1) / 2 }, 0.0, 0.0, cv::INTER_LANCZOS4);
			break;
		case Method::Pyramid:
			cv::pyrDown(src->getImage(), dst->getImage());
			break;
		default:
			break;
		}

		dst = CreateRef<Image>(dst->getImage());
	}
}