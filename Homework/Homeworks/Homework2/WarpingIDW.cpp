#include "WarpingIDW.h"


namespace Chaf
{
	void WarpingIDW::impl()
	{
		transform.resize(p_points.size(), 4);
		mu = -2.0;

		if (p_points.size() == 1)
		{
			transform << 1, 0,
				0, 1;
		}
		else if (p_points.size() == 2)	// Singular matrix
		{
			for (int i = 0; i < 2; i++)
			{
				transform(i, 0) = (q_points[1].x - q_points[0].x) / (p_points[1].x - p_points[0].x);
				transform(i, 1) = 0;
				transform(i, 2) = 0;
				transform(i, 3) = (q_points[1].y - q_points[0].y) / (p_points[1].y - p_points[0].y);
			}
		}
		else
		{
#pragma omp parallel for
			for (int i = 0; i < p_points.size(); i++)
			{
				Eigen::MatrixXd A(2, 2), B(2, 2), T_(2, 2);

				A.setZero();
				B.setZero();
				for (int j = 0; j < p_points.size(); j++)
				{
					if (i != j)
					{
						double sigma = pow(cv::norm(p_points[i] - p_points[j]), mu);
						Eigen::Matrix2d diff_p;
						Eigen::Matrix2d diff_q;
						diff_p << (double)p_points[j].x - (double)p_points[i].x, 0, (double)p_points[j].y - (double)p_points[i].y, 0;
						diff_q << (double)q_points[j].x - (double)q_points[i].x, (double)q_points[j].y - (double)q_points[i].y, 0, 0;
						A += sigma * diff_p * diff_p.transpose();
						B += sigma * diff_p * diff_q;
					}
					T_ = (A.inverse()) * B;
					transform(i, 0) = T_(0, 0);
					transform(i, 1) = T_(0, 1);
					transform(i, 2) = T_(1, 0);
					transform(i, 3) = T_(1, 1);
				}
			}
		}
	}

	cv::Point2d WarpingIDW::convert(const cv::Point2d& from)
	{
		Eigen::Vector2d convert_vactor;
		Eigen::Vector2d vector_p;
		convert_vactor.setZero();
		vector_p << from.x, from.y;

		double weight_sum = 0.0;
		std::vector<double> weights;

		for (int i = 0; i < p_points.size(); i++)
		{
			weights.push_back(pow(static_cast<double>(cv::norm(p_points[i] - from)), mu));
			weight_sum += weights[i];
		}

		if (weights.empty())
		{
			return from;
		}
		else
		{
			for (int i = 0; i < weights.size(); i++)
			{
				Eigen::MatrixXd T_ = transform.row(i).reshaped(2, 2);
				Eigen::Vector2d vector_q_i;
				Eigen::Vector2d vector_p_i;
				vector_q_i << q_points[i].x, q_points[i].y;
				vector_p_i << p_points[i].x, p_points[i].y;
				convert_vactor += (weights[i] / weight_sum) * (vector_q_i + T_ * (vector_p - vector_p_i));
			}
		}
		return { static_cast<double>(convert_vactor.x()), static_cast<double>(convert_vactor.y()) };
	}
}