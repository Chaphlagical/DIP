#include "Similarity.h"

namespace Chaf
{
	float Similarity::SSIM(Image& img1, Image& img2)
	{
		const double C1 = 6.5025, C2 = 58.5225;
		/***************************** INITS **********************************/

		cv::Mat I1, I2;
		img1.getImage().convertTo(I1, CV_32F);            // cannot calculate on one byte large values
		img2.getImage().convertTo(I2, CV_32F);

		cv::Scalar mu1, mu2, stddev1, stddev2, stddev12_, cov12;
		cv::meanStdDev(I1, mu1, stddev1);
		cv::meanStdDev(I2, mu2, stddev2);

		{
			cv::Mat tmp, tmp1, tmp2;
			cv::subtract(I1, mu1, tmp1);

			cv::subtract(I2, mu2, tmp2);
			cv::multiply(tmp1, tmp2, tmp);
			cv::meanStdDev(tmp, cov12, stddev12_);
			cv::sqrt(cov12, cov12);
		}

		cv::Scalar k1 = { 0.01f, 0.01f, 0.01f }, k2 = { 0.03f, 0.03f, 0.03f }, L = { 255.f, 255.f, 255.f };
		cv::Scalar c1, c2, c3;
		c1 = k1.mul(k1).mul(L).mul(L);
		c2 = k2.mul(k2).mul(L).mul(L);
		c3 = c2.mul(cv::Scalar(0.5f, 0.5f, 0.5f));

		cv::Scalar l, c, s;
		cv::Scalar mu12, mu11, mu22, stddev11, stddev12, stddev22, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6;

		mu12 = mu1.mul(mu2);
		mu11 = mu1.mul(mu1);
		mu22 = mu2.mul(mu2);

		stddev12 = stddev1.mul(stddev2);
		stddev11 = stddev1.mul(stddev1);
		stddev22 = stddev2.mul(stddev2);

		cv::scaleAdd(mu12, 2.f, c1, tmp1);

		cv::add(mu11, mu22, tmp2);
		cv::add(c1, tmp2, tmp2);

		cv::scaleAdd(stddev12, 2.f, c2, tmp3);

		cv::add(stddev1, stddev2, tmp4);
		cv::add(c2, tmp4, tmp4);

		cv::add(cov12, c3, tmp5);
		cv::add(stddev12, c3, tmp6);

		cv::divide(tmp1, tmp2, l);
		cv::divide(tmp3, tmp4, c);
		cv::divide(tmp5, tmp6, s);

		cv::Scalar ssim;
		ssim = l.mul(c).mul(s);

		return (ssim.val[0] + ssim.val[1] + ssim.val[2]) / 3.f;
	}

	float Similarity::PSNR(Image& img1, Image& img2)
	{
		cv::Mat diff, mat1, mat2;
		img1.getImage().convertTo(mat1, CV_32F);
		img2.getImage().convertTo(mat2, CV_32F);
		diff.convertTo(diff, CV_32F);
		cv::absdiff(mat1, mat2, diff);
		diff = diff.mul(diff);
		cv::Scalar s = cv::sum(diff);
		float sse = s.val[0] + s.val[1] + s.val[2];

		float mse = sse / (img1.getImage().channels() * img1.getImage().total());
		float psnr = 10.f * log10f((255.f * 255.f) / (mse + 1e-10f));
		return psnr;
	}
}