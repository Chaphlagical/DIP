#include "LazySnapping.h"

namespace Chaf
{
	void LazySnapping::setImage(const cv::Mat& img)
	{
		img.copyTo(m_src_img);
		m_graph = std::make_unique<GraphType>(img.rows * img.cols, img.rows * img.cols * 2);
	}

	void LazySnapping::setForeheadPoints(const std::vector<cv::Point2i>& points)
	{
		m_forehead_points.clear();
		for (auto& point : points)
		{
			m_forehead_points.insert(cv::Point2i(point.x, m_src_img.rows - point.y));
		}
		if (m_forehead_points.empty())
		{
			return;
		}

		cv::Vec3i sum = cv::Vec3i(0, 0, 0);
		for (auto& p : m_forehead_points)
		{
			sum[0] += static_cast<int>(m_src_img.at<cv::Vec3b>(p.y, p.x)[0]);
			sum[1] += static_cast<int>(m_src_img.at<cv::Vec3b>(p.y, p.x)[1]);
			sum[2] += static_cast<int>(m_src_img.at<cv::Vec3b>(p.y, p.x)[2]);
		}

		m_avg_forehead_color[0] = static_cast<uchar>(sum[0] / static_cast<int>(m_forehead_points.size()));
		m_avg_forehead_color[1] = static_cast<uchar>(sum[1] / static_cast<int>(m_forehead_points.size()));
		m_avg_forehead_color[2] = static_cast<uchar>(sum[2] / static_cast<int>(m_forehead_points.size()));
	}

	void LazySnapping::setBackgroundPoints(const std::vector<cv::Point2i>& points)
	{
		m_background_points.clear();
		for (auto& point : points)
		{
			m_background_points.insert(cv::Point2i(point.x, m_src_img.rows - point.y));
		}
		if (m_background_points.empty())
		{
			return;
		}

		cv::Vec3i sum = cv::Vec3i(0, 0, 0);
		for (auto& p : m_background_points)
		{
			sum[0] += static_cast<int>(m_src_img.at<cv::Vec3b>(p.y, p.x)[0]);
			sum[1] += static_cast<int>(m_src_img.at<cv::Vec3b>(p.y, p.x)[1]);
			sum[2] += static_cast<int>(m_src_img.at<cv::Vec3b>(p.y, p.x)[2]);
		}

		m_avg_background_color[0] = static_cast<uchar>(sum[0] / static_cast<int>(m_background_points.size()));
		m_avg_background_color[1] = static_cast<uchar>(sum[1] / static_cast<int>(m_background_points.size()));
		m_avg_background_color[2] = static_cast<uchar>(sum[2] / static_cast<int>(m_background_points.size()));
	}

	int LazySnapping::runMaxflow()
	{
		//int point_index = 0;
		for (int r = 0; r < m_src_img.rows; r++)
		{
			for (int c = 0; c < m_src_img.cols; c++)
			{
				cv::Vec3b p = m_src_img.at<cv::Vec3b>(r, c);

				float e1[2] = { 0.f, 0.f };
				if (m_forehead_points.find(cv::Point2i(c, r)) != m_forehead_points.end())
				{
					e1[0] = 0;
					e1[1] = std::numeric_limits<float>::infinity();
				}
				else if (m_background_points.find(cv::Point2i(c, r)) != m_background_points.end())
				{
					e1[0] = std::numeric_limits<float>::infinity();
					e1[1] = 0;
				}
				else
				{
					auto [x, y] = getE1(p);
					e1[0] = x;
					e1[1] = y;
				}
				// Add node
				int point_index = m_graph->add_node();
				m_graph->add_tweights(point_index, e1[0], e1[1]);
				// Add edge
				if (r > 0 && c > 0)
				{
					float e2 = getE2(p, m_src_img.at<cv::Vec3b>(r, c - 1));
					m_graph->add_edge(point_index, point_index - 1, e2, e2);
					e2 = getE2(p, m_src_img.at<cv::Vec3b>(r - 1, c));
					m_graph->add_edge(point_index, point_index - m_src_img.cols, e2, e2);
				}
				point_index++;
			}
		}

		return m_graph->maxflow();
	}

	cv::Mat LazySnapping::getMask()
	{
		auto mask = cv::Mat(m_src_img.rows, m_src_img.cols, CV_8UC3);
		int point_index = 0;

		for (int r = 0; r < m_src_img.rows; r++)
		{
			for (int c = 0; c < m_src_img.cols; c++)
			{
				if (m_graph->what_segment(point_index) == GraphType::SOURCE)
				{
					mask.at<cv::Vec3b>(r, c) = cv::Vec3b(0, 0, 0);
				}
				else
				{
					mask.at<cv::Vec3b>(r, c) = cv::Vec3b(255, 255, 255);
				}
				point_index++;
			}
		}

		return mask;
	}

	cv::Mat LazySnapping::drawContours(const cv::Mat& img, const cv::Mat& mask)
	{
		cv::Mat binary, result;
		img.copyTo(result);

		cv::cvtColor(mask, binary, cv::COLOR_BGR2GRAY);

		cv::Canny(binary, binary, 50, 150, 3);

		cv::threshold(binary, binary, 100, 255, cv::THRESH_BINARY);
		binary.convertTo(binary, CV_8UC1);
		std::vector<std::vector<cv::Point> > contours;
		std::vector<cv::Vec4i> hierarchy;
		cv::findContours(binary, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
		std::sort(contours.begin(), contours.end(), [](std::vector<cv::Point>& lhs, std::vector<cv::Point>& rhs) {return lhs.size() > rhs.size(); });
		cv::drawContours(result, contours, -1, cv::Vec3b(0, 0, 255), 3);
		return result;
	}

	float LazySnapping::colorDistance(const cv::Vec3b& lhs, const cv::Vec3b& rhs)
	{
		cv::Vec3f p;
		p[0] = static_cast<float>(lhs[0]) - static_cast<float>(rhs[0]);
		p[1] = static_cast<float>(lhs[1]) - static_cast<float>(rhs[1]);
		p[2] = static_cast<float>(lhs[2]) - static_cast<float>(rhs[2]);
		return sqrtf(p[0] * p[0] + p[1] * p[1] + p[2] * p[2]);
	}

	float LazySnapping::minDistance(const cv::Vec3b& color, const std::vector<cv::Point2i>& points)
	{
		float distance = std::numeric_limits<float>::max();
		for (auto& p : points)
		{
			float d = colorDistance(m_src_img.at<cv::Vec3b>(p.y, p.x), color);
			if (distance > d)
			{
				distance = d;
			}
		}
		return distance;
	}

	std::pair<float, float> LazySnapping::getE1(const cv::Vec3b& color)
	{
		float df = colorDistance(color, m_avg_forehead_color);
		float db = colorDistance(color, m_avg_background_color);
		return std::make_pair(df / (db + df), db / (db + df));
	}

	float LazySnapping::getE2(const cv::Vec3b& color1, const cv::Vec3b& color2)
	{
		cv::Vec3f p;
		p[0] = static_cast<float>(color1[0]) - static_cast<float>(color2[0]);
		p[1] = static_cast<float>(color1[1]) - static_cast<float>(color2[1]);
		p[2] = static_cast<float>(color1[2]) - static_cast<float>(color2[2]);

		const float EPSILON = 0.01;
		float lambda = 100;
		//return 0;
		return lambda / (EPSILON + p[0] * p[0] + p[1] * p[1] + p[2] * p[2]);
	}
}