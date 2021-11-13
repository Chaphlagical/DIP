#include "Upsampling.h"

#include <opencv2/dnn_superres.hpp>

namespace Chaf
{
	inline cv::Mat upsampling_dnn(cv::Mat img, const std::string& model_name, const std::string& model_path, int scale)
	{
		cv::dnn_superres::DnnSuperResImpl sr;
		sr.readModel(model_path);
		sr.setModel(model_name, scale);

		cv::Mat output;
		sr.upsample(img, output);
		return output;
	}

	void Upsampling::upsampling(Ref<Image>& src, Ref<Image>& dst, Method method)
	{
		switch (method)
		{
		case Method::Nearest:
			cv::resize(src->getImage(), dst->getImage(), { static_cast<int>(src->getWidth()) * 2, static_cast<int>(src->getHeight()) * 2 }, 0.0, 0.0, cv::INTER_NEAREST);
			break;
		case Method::Linear:
			cv::resize(src->getImage(), dst->getImage(), { static_cast<int>(src->getWidth()) * 2, static_cast<int>(src->getHeight()) * 2 }, 0.0, 0.0, cv::INTER_LINEAR);
			break;
		case Method::Cubic:
			cv::resize(src->getImage(), dst->getImage(), { static_cast<int>(src->getWidth()) * 2, static_cast<int>(src->getHeight()) * 2 }, 0.0, 0.0, cv::INTER_CUBIC);
			break; 
		case Method::Lanczos:
			cv::resize(src->getImage(), dst->getImage(), { static_cast<int>(src->getWidth()) * 2, static_cast<int>(src->getHeight()) * 2 }, 0.0, 0.0, cv::INTER_LANCZOS4);
			break;
		case Method::Pyramid:
			cv::pyrUp(src->getImage(), dst->getImage());
			break;
		case Method::EDSR:
			dst->getImage() = upsampling_dnn(src->getImage(), "edsr", std::string(PROJECT_SOURCE_DIR) + "../assets/dnn_model/EDSR_x2.pb", 2);
			break;
		case Method::ESPCN:
			dst->getImage() = upsampling_dnn(src->getImage(), "espcn", std::string(PROJECT_SOURCE_DIR) + "../assets/dnn_model/ESPCN_x2.pb", 2);
			break;
		case Method::FSRCNN:
			dst->getImage() = upsampling_dnn(src->getImage(), "fsrcnn", std::string(PROJECT_SOURCE_DIR) + "../assets/dnn_model/FSRCNN_x2.pb", 2);
			break;
		case Method::LapSRN:
			dst->getImage() = upsampling_dnn(src->getImage(), "lapsrn", std::string(PROJECT_SOURCE_DIR) + "../assets/dnn_model/LapSRN_x2.pb", 2);
			break;
		default:
			break;
		}

		dst = CreateRef<Image>(dst->getImage());
	}
}