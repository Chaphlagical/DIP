#pragma once

#include <Image.h>

#include <Base.h>

namespace Chaf
{
	enum class SeamCarvingDirection
	{
		Vertical,
		Horizontal,
		Proportion
	};

	class SeamCarving
	{
	public:
		static void genEnergyMap(const cv::Mat& img, cv::Mat& energy);
		// Return energy
		static float findSeamVertical(const cv::Mat& energy, std::vector<int>& seam);
		static void narrowSeamVertical(const cv::Mat&src, cv::Mat& dst, const std::vector<int>& seam);
		// Return energy
		static float findSeamHorizontal(const cv::Mat& energy, std::vector<int>& seam);
		static void narrowSeamHorizontal(const cv::Mat& src, cv::Mat& dst, const std::vector<int>& seam);

		static void findSeamVertical(const cv::Mat& energy, std::vector<std::vector<int>>& seam, int count);
		static void enlargeSeamVertical(const cv::Mat& src, cv::Mat& dst, const std::vector<int>& seam);
		static void enlargeSeamVertical(const cv::Mat& src, cv::Mat& dst, const std::vector<std::vector<int>>& seams);

		static void findSeamHorizontal(const cv::Mat& energy, std::vector<std::vector<int>>& seam, int count);
		static void enlargeSeamHorizontal(const cv::Mat& src, cv::Mat& dst, const std::vector<int>& seam);
		static void enlargeSeamHorizontal(const cv::Mat& src, cv::Mat& dst, const std::vector<std::vector<int>>& seams);

		inline static cv::Point2d clear_rect[2] = {};
	};

}