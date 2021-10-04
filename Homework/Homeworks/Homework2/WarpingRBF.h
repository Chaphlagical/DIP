#pragma once

#include "Warping.h"

__pragma(warning(push, 0))
#include <Eigen/Eigen>
__pragma(warning(pop))

namespace Chaf
{
	class WarpingRBF : public Warping
	{
	private:
		virtual cv::Point2d convert(const cv::Point2d& from) override;

		virtual void impl() override;

	private:
		Eigen::VectorXd weight_x, weight_y;
		std::vector <double> radius;
		double mu = 0.5;
	};
}