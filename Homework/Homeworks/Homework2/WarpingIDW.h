#pragma once

#include "Warping.h"

__pragma(warning(push, 0))
#include <Eigen/Eigen>
__pragma(warning(pop))

namespace Chaf
{
	class WarpingIDW: public Warping
	{
	private:
		virtual cv::Point2d convert(const cv::Point2d& from) override;

		virtual void impl() override;

	private:
		Eigen::MatrixXd transform;
		double mu = 2.0;
	};
}