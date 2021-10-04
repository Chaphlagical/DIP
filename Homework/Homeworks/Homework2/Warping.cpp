#include "Warping.h"

#include <ANN/ANN.h>

#include <omp.h>

namespace Chaf
{
	cv::Mat Warping::warp(const Image& src_image, Image& dst_image, const std::vector<cv::Point2d>& src_points, const std::vector<cv::Point2d>& dst_points)
	{
		init(src_image, src_points, dst_points);
		auto mask = cv::Mat(src_image.getImage().rows, src_image.getImage().cols, CV_8U);

		mask.setTo(0);

		impl();

		dst_image.getImage().resize(src_image.getImage().rows, src_image.getImage().cols);
		dst_image.getImage().setTo(255);

#pragma omp parallel for
		for (int i = 0; i < static_cast<int>(src_image.getWidth()); i++)
		{
#pragma omp parallel for
			for (int j = 0; j < static_cast<int>(src_image.getHeight()); j++)
			{
				auto convert_point = convert({ static_cast<double>(i), static_cast<double>(j) });
				if (convert_point.x > 0 && convert_point.x < src_image.getWidth())
				{
					if (convert_point.y > 0 && convert_point.y < src_image.getHeight())
					{
						dst_image.getImage().at<cv::Vec3b>(static_cast<int>(convert_point.y), static_cast<int>(convert_point.x)) = src_image.getImage().at<cv::Vec3b>(j, i);

						mask.at<uint8_t>(static_cast<int>(convert_point.y), static_cast<int>(convert_point.x)) = 1;
					}
				}
			}
		}

		return mask;
	}

	void Warping::fix(Image& image, const cv::Mat& mask, uint32_t threshold, uint32_t count)
	{
		ANNpointArray dataPts = annAllocPts(static_cast<int>(image.getHeight() * image.getWidth()), 2);
		int nPts = 0;

		image.getImage().convertTo(image.getImage(), CV_64FC3);

		for (int i = 0; i < static_cast<int>(image.getWidth()); i++)
		{
			for (int j = 0; j < static_cast<int>(image.getHeight()); j++)
			{
				if (mask.at<uint8_t>(j, i) == 1)
				{
					dataPts[nPts][0] = i;
					dataPts[nPts][1] = j;
					nPts++;
				}
			}
		}

		ANNkd_tree* kdtree = new ANNkd_tree(dataPts, nPts, 2);

		for (int i = 0; i < static_cast<int>(image.getWidth()); i++)
		{
			for (int j = 0; j < static_cast<int>(image.getHeight()); j++)
			{
				cv::Vec3d color = { 0.0,0.0,0.0 };

				ANNpoint pt = annAllocPt(2);
				ANNidxArray index = new ANNidx[count];
				ANNdistArray dist = new ANNdist[count];
				pt[0] = i;
				pt[1] = j;
				kdtree->annkSearch(pt, count, index, dist, 0);

				int real_count = count;
				for (int m = 0; m < count; m++)
				{
					int x = static_cast<int>(dataPts[index[m]][0]);
					int y = static_cast<int>(dataPts[index[m]][1]);
					if (fabs(x - i) <= threshold && fabs(y - j) <= threshold)
					{
						color += image.getImage().at<cv::Vec3d>(y, x);
					}
					else
					{
						real_count--;
					}
				}
				if (real_count > 0)
				{
					image.getImage().at<cv::Vec3d>(j, i) = color / static_cast<double>(real_count);
				}
				delete[] index;
				delete[] dist;
			}
		}

		image.getImage().convertTo(image.getImage(), CV_8UC3);

		delete kdtree;
	}

	void Warping::init(const Image& src_image, const std::vector<cv::Point2d>& src_points, const std::vector<cv::Point2d>& dst_points)
	{
		p_points.resize(src_points.size());
		q_points.resize(dst_points.size());

		for (uint32_t i = 0; i < p_points.size(); i++)
		{
			p_points[i].x = src_points[i].x * static_cast<double>(src_image.getWidth());
			p_points[i].y = (1.0 - src_points[i].y) * static_cast<double>(src_image.getHeight());
			q_points[i].x = dst_points[i].x * static_cast<double>(src_image.getWidth());
			q_points[i].y = (1.0 - dst_points[i].y) * static_cast<double>(src_image.getHeight());
		}
	}
}