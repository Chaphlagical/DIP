#pragma once

#include <Engine/core.h>
#include <Image.h>

namespace Chaf
{
	class Warping
	{
	public:
		Warping() = default;

		~Warping() = default;

		cv::Mat warp(const Image& src_image, Image& dst_image, const std::vector<cv::Point2d>& src_points, const std::vector<cv::Point2d>& dst_points);

		// mask: 1 channel mask
		void fix(Image& image, const cv::Mat& mask, uint32_t threshold = 2, uint32_t count = 4);

	protected:
		void init(const Image& src_image, const std::vector<cv::Point2d>& src_points, const std::vector<cv::Point2d>& dst_points);

		virtual cv::Point2d convert(const cv::Point2d& from) = 0;

		virtual void impl() = 0;

	protected:
		std::vector<cv::Point2d> p_points;
		std::vector<cv::Point2d> q_points;
	};
}