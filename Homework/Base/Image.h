#pragma once

#include <string>

#include <glad/glad.h>
#include <opencv2/opencv.hpp>

namespace Chaf
{
	class Image
	{
	public:
		Image(const std::string& path);

		Image(const cv::Mat& image);

		~Image();

		cv::Mat& getImage();

		const cv::Mat& getImage() const;

		GLuint getTextureID() const;

		const uint32_t getWidth() const;

		const uint32_t getHeight() const;

		const std::string& getPath() const;

		void save(const std::string& path) const;

	private:
		std::string m_path = "";

		GLuint m_texture_id = 0;
		cv::Mat m_image;
	};
}