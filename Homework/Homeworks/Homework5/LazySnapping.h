#pragma once

#include "maxflow/graph.h"

#include "Image.h"

#include <unordered_set>

namespace std {
	template<> struct hash<cv::Point2i> {
		size_t operator()(cv::Point2i const& point) const
		{
			return ((hash<int>()(point.x) ^
				(hash<int>()(point.y) << 1)) >> 1);
		}
	};
}

namespace Chaf
{
	using GraphType = Graph<float, float, float>;

	class LazySnapping
	{
	public:
		LazySnapping() = default;

		~LazySnapping() = default;

		void setImage(const cv::Mat& img);

		void setForeheadPoints(const std::vector<cv::Point2i>& points);

		void setBackgroundPoints(const std::vector<cv::Point2i>& points);

		int runMaxflow();

		cv::Mat getMask();

		cv::Mat drawContours(const cv::Mat& img, const cv::Mat& mask);

	private:
		float colorDistance(const cv::Vec3b& lhs, const cv::Vec3b& rhs);
		
		float minDistance(const cv::Vec3b& color, const std::vector<cv::Point2i>& points);

		std::pair<float, float> getE1(const cv::Vec3b& color);

		float getE2(const cv::Vec3b& color1, const cv::Vec3b& color2);

	private:
		std::unordered_set<cv::Point2i> m_forehead_points;
		std::unordered_set<cv::Point2i> m_background_points;
		cv::Mat m_src_img;
		cv::Vec3b m_avg_forehead_color = cv::Vec3b(0, 0, 0);
		cv::Vec3b m_avg_background_color = cv::Vec3b(0, 0, 0);
		std::unique_ptr<GraphType> m_graph = nullptr;
	};
}

