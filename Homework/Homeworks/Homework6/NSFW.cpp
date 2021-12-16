#include "NSFW.h"

#include <opencv2/dnn/dnn.hpp>

namespace Chaf
{
	std::string NSFW::check(const cv::Mat& img)
	{
		auto net = cv::dnn::readNetFromONNX(PROJECT_SOURCE_DIR + std::string("../assets/dnn_model/nsfw_resnet50.onnx"));

		// preprocessing
		cv::Mat blob = cv::dnn::blobFromImage(img, 0.003921569, cv::Size(224, 224), cv::Scalar(123.675, 116.28, 103.53), true, true);
		cv::divide(blob, cv::Scalar(0.229, 0.224, 0.225), blob);

		net.setInput(blob);
		auto prob = net.forward();

		cv::Point classIdPoint;
		double confidence;
		minMaxLoc(prob, 0, &confidence, 0, &classIdPoint);
		int classId = classIdPoint.x;

		std::array<std::string, 5> class_names = { "drawings", "hentai", "neutral", "porn","sexy" };

		return class_names[classId];
	}
}