#include "WarpingRBF.h"

namespace Chaf
{
	cv::Point2d WarpingRBF::convert(const cv::Point2d& from)
	{
		double convert_x = 0;
		double convert_y = 0;
		if (p_points.size() > 1)
		{
#pragma omp parallel for
			for (int i = 0; i < p_points.size(); i++)
			{
				convert_x += weight_x(i) * pow(pow(cv::norm(p_points[i] - from), 2) + radius[i] * radius[i], mu);
				convert_y += weight_y(i) * pow(pow(cv::norm(p_points[i] - from), 2) + radius[i] * radius[i], mu);
			}
		}
		else
		{
			convert_x = q_points[0].x - p_points[0].x;
			convert_y = q_points[0].y - p_points[0].y;
		}

		return { convert_x + from.x ,convert_y + from.y };
	}

	void WarpingRBF::impl()
	{
		if (p_points.size() > 1)
		{
			for (int i = 0; i < p_points.size(); i++)
			{
				double d_min = std::numeric_limits<double>::max();
				for (int j = 0; j < p_points.size(); j++)
				{
					if (i != j)
					{
						double dist = cv::norm(p_points[i] - p_points[j]);
						if (dist < d_min)
						{
							d_min = dist;
						}
					}
				}
				radius.push_back(d_min);
			}
		}
		else
		{
			radius.push_back(0.0);
		}

		Eigen::MatrixXd A(p_points.size(), p_points.size());
		Eigen::VectorXd b_x(p_points.size());
		Eigen::VectorXd b_y(p_points.size());
		A.setZero(); b_x.setZero(); b_y.setZero();

		if (p_points.size() > 1)
		{
			for (int i = 0; i < A.rows(); i++)
			{
				for (int j = 0; j < A.cols(); j++)
				{
					A(i, j) = pow(pow(cv::norm(p_points[i] - p_points[j]), 2) + radius[j] * radius[j], mu);
				}
				b_x(i) = q_points[i].x - p_points[i].x;
				b_y(i) = q_points[i].y - p_points[i].y;
			}
			weight_x.resize(p_points.size());
			weight_y.resize(p_points.size());
			weight_x = A.inverse() * b_x;
			weight_y = A.inverse() * b_y;
		}
	}
}