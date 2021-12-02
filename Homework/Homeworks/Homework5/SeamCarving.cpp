#include "SeamCarving.h"

namespace Chaf
{
	void SeamCarving::genEnergyMap(const cv::Mat& img, cv::Mat& energy)
	{
		cv::Mat sobel_x, sobel_y, gray_energy;
		cv::cvtColor(img, gray_energy, cv::COLOR_BGR2GRAY);
		cv::Sobel(gray_energy, sobel_x, CV_32F, 1, 0, 3);
		cv::convertScaleAbs(sobel_x, sobel_x);
		cv::Sobel(gray_energy, sobel_y, CV_32F, 0, 1, 3);
		cv::convertScaleAbs(sobel_y, sobel_y);
		cv::addWeighted(sobel_x, 0.5, sobel_y, 0.5, 0, energy);
		energy.convertTo(energy, CV_32FC1);
	}

	float SeamCarving::findSeamVertical(const cv::Mat& energy, std::vector<int>& seam)
	{
		const uint32_t height = energy.rows;
		const uint32_t width = energy.cols;

		std::vector<std::vector<float>> dp_m;
		std::vector<std::vector<int>> dp_path;

		dp_m.resize(height, std::vector<float>(width, 0.f));
		dp_path.resize(height, std::vector<int>(width, 0.f));

		for (int i = 0; i < width; i++)
		{
			dp_m[0][i] = energy.at<float>(0, i);
			dp_path[0][i] = i;
		}

		for (int i = 1; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				// Find min energy
				float left_upper = j - 1 >= 0 ? dp_m[i - 1][j - 1] : std::numeric_limits<float>::max();
				float right_upper = j + 1 < width ? dp_m[i - 1][j + 1] : std::numeric_limits<float>::max();
				float middle_upper = dp_m[i - 1][j];
				float min_energy = std::min(middle_upper, std::min(left_upper, right_upper));

				int parent_idx = j;
				if (std::fabs(min_energy - left_upper) < 1e-7)
				{
					parent_idx = j - 1;
				}
				else if (std::fabs(min_energy - right_upper) < 1e-7)
				{
					parent_idx = j + 1;
				}

				dp_m[i][j] = energy.at<float>(i, j) + min_energy;
				dp_path[i][j] = parent_idx;
			}
		}

		seam.resize(height, 0);

		float E = 0;

		int col_idx = std::min_element(dp_m[height - 1].begin(), dp_m[height - 1].end()) - dp_m[height - 1].begin();
		seam[height - 1] = col_idx;
		E = dp_m[height - 1][col_idx];
		for (int i = height - 1; i > 0; i--)
		{
			col_idx = dp_path[i][col_idx];
			seam[i - 1] = col_idx;
			E += dp_m[i - 1][col_idx];
		}

		return E / (float)dp_m.size();
	}

	void SeamCarving::findSeamVertical(const cv::Mat& energy, std::vector<std::vector<int>>& seams, int count)
	{
		const uint32_t height = energy.rows;
		const uint32_t width = energy.cols;

		seams.resize(count, std::vector<int>(height, 0));

		std::vector<std::vector<float>> dp_m;
		std::vector<std::vector<int>> dp_path;

		dp_m.resize(height, std::vector<float>(width, 0.f));
		dp_path.resize(height, std::vector<int>(width, 0.f));

		cv::Mat tmp = energy;

		for (int k = 0; k < count; k++)
		{
			for (int i = 0; i < width; i++)
			{
				dp_m[0][i] = tmp.at<float>(0, i);
				dp_path[0][i] = i;
			}

			for (int i = 1; i < height; i++)
			{
				for (int j = 0; j < width; j++)
				{
					// Find min energy
					float left_upper = j - 1 >= 0 ? dp_m[i - 1][j - 1] : std::numeric_limits<float>::max();
					float right_upper = j + 1 < width ? dp_m[i - 1][j + 1] : std::numeric_limits<float>::max();
					float middle_upper = dp_m[i - 1][j];
					float min_energy = std::min(middle_upper, std::min(left_upper, right_upper));

					int parent_idx = j;
					if (std::fabs(min_energy - left_upper) < 1e-7)
					{
						parent_idx = j - 1;
					}
					else if (std::fabs(min_energy - right_upper) < 1e-7)
					{
						parent_idx = j + 1;
					}

					dp_m[i][j] = tmp.at<float>(i, j) + min_energy;
					dp_path[i][j] = parent_idx;
				}
			}

			int col_idx = std::min_element(dp_m[height - 1].begin(), dp_m[height - 1].end()) - dp_m[height - 1].begin();
			seams[k][height - 1] = col_idx;
			for (int i = height - 1; i > 0; i--)
			{
				col_idx = dp_path[i][col_idx];
				tmp.at<float>(i, col_idx) = std::numeric_limits<float>::max();
				seams[k][i - 1] = col_idx;
			}
		}
	}

	void SeamCarving::enlargeSeamVertical(const cv::Mat& src, cv::Mat& dst, const std::vector<int>& seam)
	{
		dst = cv::Mat(src.rows, src.cols + 1, CV_8UC3);
		for (int i = 0; i < src.rows; i++)
		{
			int idx = 0;
			for (int j = 0; j < src.cols + 1; j++)
			{
				if (j != seam[i])
				{
					dst.at<cv::Vec3b>(i, j) = src.at<cv::Vec3b>(i, idx);
					idx++;
				}
				else
				{
					cv::Vec3b val1 = src.at<cv::Vec3b>(i, idx);
					cv::Vec3b val2 = src.at<cv::Vec3b>(i, idx > 1 ? idx - 1 : 0);
					cv::Vec3b val3 = src.at<cv::Vec3b>(i, idx < src.cols - 1 ? idx + 1 : src.cols - 1);
					int x = ((int)val1[0] + (int)val2[0] + (int)val3[0]) / 3;
					int y = ((int)val1[1] + (int)val2[1] + (int)val3[1]) / 3;
					int z = ((int)val1[2] + (int)val2[2] + (int)val3[2]) / 3;
					dst.at<cv::Vec3b>(i, j) = cv::Vec3b((uchar)x, (uchar)y, (uchar)z);
				}
			}
		}
	}

	void SeamCarving::enlargeSeamVertical(const cv::Mat& src, cv::Mat& dst, const std::vector<std::vector<int>>& seams)
	{
		dst = src;
		for (int i = seams.size() - 1; i >= 0; i--)
		{
			auto tmp = dst.clone();
			enlargeSeamVertical(tmp, dst, seams[i]);
		}
	}

	void SeamCarving::narrowSeamVertical(const cv::Mat& src, cv::Mat& dst, const std::vector<int>& seam)
	{
		dst = cv::Mat(src.rows, src.cols - 1, CV_8UC3);
		for (int i = 0; i < src.rows; i++)
		{
			int idx = 0;
			for (int j = 0; j < src.cols; j++)
			{
				if (j != seam[i])
				{
					dst.at<cv::Vec3b>(i, idx) = src.at<cv::Vec3b>(i, j);
					idx++;
				}
			}
		}
	}

	float SeamCarving::findSeamHorizontal(const cv::Mat& energy, std::vector<int>& seam)
	{
		const uint32_t height = energy.rows;
		const uint32_t width = energy.cols;

		std::vector<std::vector<float>> dp_m;
		std::vector<std::vector<int>> dp_path;

		dp_m.resize(width, std::vector<float>(height, 0.f));
		dp_path.resize(width, std::vector<int>(height, 0.f));

		for (int i = 0; i < height; i++)
		{
			dp_m[0][i] = energy.at<float>(i, 0);
			dp_path[0][i] = i;
		}

		for (int i = 1; i < width; i++)
		{
			for (int j = 0; j < height; j++)
			{
				// Find min energy
				float up_upper = j - 1 >= 0 ? dp_m[i - 1][j - 1] : std::numeric_limits<float>::max();
				float down_upper = j + 1 < height ? dp_m[i - 1][j + 1] : std::numeric_limits<float>::max();
				float middle_upper = dp_m[i - 1][j];
				float min_energy = std::min(middle_upper, std::min(up_upper, down_upper));

				int parent_idx = j;
				if (std::fabs(min_energy - up_upper) < 1e-7)
				{
					parent_idx = j - 1;
				}
				else if (std::fabs(min_energy - down_upper) < 1e-7)
				{
					parent_idx = j + 1;
				}

				dp_m[i][j] = energy.at<float>(j, i) + min_energy;
				dp_path[i][j] = parent_idx;
			}
		}

		seam.resize(width, 0);
		float E = 0;

		int row_idx = std::min_element(dp_m[width - 1].begin(), dp_m[width - 1].end()) - dp_m[width - 1].begin();
		seam[width - 1] = row_idx;
		E += dp_m[width - 1][row_idx];
		for (int i = width - 1; i > 0; i--)
		{
			row_idx = dp_path[i][row_idx];
			seam[i - 1] = row_idx;
			E += dp_m[i - 1][row_idx];
		}

		return E / (float)dp_m.size();
	}

	void SeamCarving::findSeamHorizontal(const cv::Mat& energy, std::vector<std::vector<int>>& seams, int count)
	{
		const uint32_t height = energy.rows;
		const uint32_t width = energy.cols;

		seams.resize(count, std::vector<int>(width, 0));

		std::vector<std::vector<float>> dp_m;
		std::vector<std::vector<int>> dp_path;

		dp_m.resize(width, std::vector<float>(height, 0.f));
		dp_path.resize(width, std::vector<int>(height, 0.f));

		cv::Mat tmp = energy;

		for (int k = 0; k < count; k++)
		{
			for (int i = 0; i < height; i++)
			{
				dp_m[0][i] = tmp.at<float>(i, 0);
				dp_path[0][i] = i;
			}

			for (int i = 1; i < width; i++)
			{
				for (int j = 0; j < height; j++)
				{
					// Find min energy
					float up_upper = j - 1 >= 0 ? dp_m[i - 1][j - 1] : std::numeric_limits<float>::max();
					float down_upper = j + 1 < height ? dp_m[i - 1][j + 1] : std::numeric_limits<float>::max();
					float middle_upper = dp_m[i - 1][j];
					float min_energy = std::min(middle_upper, std::min(up_upper, down_upper));

					int parent_idx = j;
					if (std::fabs(min_energy - up_upper) < 1e-7)
					{
						parent_idx = j - 1;
					}
					else if (std::fabs(min_energy - down_upper) < 1e-7)
					{
						parent_idx = j + 1;
					}

					dp_m[i][j] = tmp.at<float>(j, i) + min_energy;
					dp_path[i][j] = parent_idx;
				}
			}

			int row_idx = std::min_element(dp_m[width - 1].begin(), dp_m[width - 1].end()) - dp_m[width - 1].begin();
			seams[k][width - 1] = row_idx;
			for (int i = width - 1; i > 0; i--)
			{
				row_idx = dp_path[i][row_idx];
				tmp.at<float>(row_idx, i) = std::numeric_limits<float>::max();
				seams[k][i - 1] = row_idx;
			}
		}
	}

	void SeamCarving::enlargeSeamHorizontal(const cv::Mat& src, cv::Mat& dst, const std::vector<int>& seam)
	{
		dst = cv::Mat(src.rows + 1, src.cols, CV_8UC3);
		for (int i = 0; i < src.cols; i++)
		{
			int idx = 0;
			for (int j = 0; j < src.rows + 1; j++)
			{
				if (j != seam[i])
				{
					dst.at<cv::Vec3b>(j, i) = src.at<cv::Vec3b>(idx, i);
					idx++;
				}
				else
				{
					cv::Vec3b val1 = src.at<cv::Vec3b>(idx, i);
					cv::Vec3b val2 = src.at<cv::Vec3b>(idx > 1 ? idx - 1 : 0, i);
					cv::Vec3b val3 = src.at<cv::Vec3b>(idx < src.rows - 1 ? idx + 1 : src.rows - 1, i);
					int x = ((int)val1[0] + (int)val2[0] + (int)val3[0]) / 3;
					int y = ((int)val1[1] + (int)val2[1] + (int)val3[1]) / 3;
					int z = ((int)val1[2] + (int)val2[2] + (int)val3[2]) / 3;
					dst.at<cv::Vec3b>(j, i) = cv::Vec3b((uchar)x, (uchar)y, (uchar)z);
				}
			}
		}
	}

	void SeamCarving::enlargeSeamHorizontal(const cv::Mat& src, cv::Mat& dst, const std::vector<std::vector<int>>& seams)
	{
		dst = src;
		for (int i = seams.size() - 1; i >= 0; i--)
		{
			auto tmp = dst.clone();
			enlargeSeamHorizontal(tmp, dst, seams[i]);
		}
	}

	void SeamCarving::narrowSeamHorizontal(const cv::Mat& src, cv::Mat& dst, const std::vector<int>& seam)
	{
		dst = cv::Mat(src.rows - 1, src.cols, CV_8UC3);
		for (int i = 0; i < src.cols; i++)
		{
			int idx = 0;
			for (int j = 0; j < src.rows; j++)
			{
				if (j != seam[i])
				{
					dst.at<cv::Vec3b>(idx, i) = src.at<cv::Vec3b>(j, i);
					idx++;
				}

			}
		}
	}
}