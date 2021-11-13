#include "HW4Layer.h"

#include "Downsampling.h"
#include "Upsampling.h"
#include "Similarity.h"

#include <Editor/basic.h>

#include <imgui.h>
#include <implot.h>

#include <opencv2/dnn_superres.hpp>

namespace Chaf
{
	HW4Layer::HW4Layer()
	{

	}

	void HW4Layer::OnAttach()
	{
	}

	void HW4Layer::OnDetach()
	{
	}

	void HW4Layer::OnUpdate(Timestep timestep)
	{
	}

	void HW4Layer::OnImGuiRender()
	{
		EditorBasic::GetFileDialog("Homework4##Select Source Image", ".png,.jpg,.bmp,.jpeg", [this](const std::string& path) {
			m_src_image = CreateRef<Image>(path);
			m_downsampled_image = CreateRef<Image>(path);
			m_upsampled_image = CreateRef<Image>(path);
			Downsampling::downsampling(m_src_image, m_downsampled_image, Downsampling::Method::Nearest);
			Upsampling::upsampling(m_downsampled_image, m_upsampled_image, Upsampling::Method::Nearest);
			});

		ImGui::Begin("Homework4");
		ImGui::Columns(2, "Homework4");

		if (ImGui::Button("Load Source"))
		{
			EditorBasic::SetPopupFlag("Homework4##Select Source Image");
		}

		const char* const downsampling_methods[] = { "Nearest", "Linear", "Cubic", "Lanczos", "Pyramid" };
		static int downsampling_method = 0;

		const char* const upsampling_methods[] = { "Nearest", "Linear", "Cubic", "Lanczos", "Pyramid", "EDSR", "ESPCN", "FSRCNN", "LapSRN"};
		static int upsampling_method = 0;

		if (ImGui::Combo("Downsampling", &downsampling_method, downsampling_methods, 5) && m_src_image)
		{
			Downsampling::downsampling(m_src_image, m_downsampled_image, static_cast<Downsampling::Method>(downsampling_method));
			Upsampling::upsampling(m_downsampled_image, m_upsampled_image, static_cast<Upsampling::Method>(upsampling_method));
			cv::Mat diff;
			cv::absdiff(m_upsampled_image->getImage(), m_src_image->getImage(), diff);
			m_diff_image = CreateRef<Image>(diff);
			m_ssim = Similarity::SSIM(*m_src_image, *m_upsampled_image);
			m_psnr = Similarity::PSNR(*m_src_image, *m_upsampled_image);
		}

		if (ImGui::Combo("Upsampling", &upsampling_method, upsampling_methods, 9) && m_src_image)
		{
			Upsampling::upsampling(m_downsampled_image, m_upsampled_image, static_cast<Upsampling::Method>(upsampling_method));
			cv::Mat diff;
			cv::absdiff(m_upsampled_image->getImage(), m_src_image->getImage(), diff);
			m_diff_image = CreateRef<Image>(diff);
			m_ssim = Similarity::SSIM(*m_src_image, *m_upsampled_image);
			m_psnr = Similarity::PSNR(*m_src_image, *m_upsampled_image);
		}

		show_texture(m_src_image, "image", ImGui::GetWindowContentRegionWidth() / 3.f);
		ImGui::NewLine();
		show_texture(m_diff_image, "difference", ImGui::GetWindowContentRegionWidth() / 3.f);

		auto draw_list = ImGui::GetWindowDrawList();

		ImGui::NextColumn();

		ImGui::Text("Result");

		if (m_src_image)
		{
			ImGui::Text("PSNR: %f", m_psnr);
			ImGui::Text("SSIM: %f", m_ssim);
		}

		show_texture(m_downsampled_image, "down sampled", ImGui::GetWindowContentRegionWidth() / 6.f);
		ImGui::SameLine();
		show_texture(m_upsampled_image, "up sampled", ImGui::GetWindowContentRegionWidth() / 3.f);

		ImGui::End();
	}

	void HW4Layer::OnEvent(Event& event)
	{
	}

	void HW4Layer::show_texture(Ref<Image>& image, const char* label, float width)
	{
		if (!image)
		{
			return;
		}

		ImGui::Image((void*)image->getTextureID(), { width, width * static_cast<float>(image->getHeight()) / static_cast<float>(image->getWidth()) }, ImVec2(0, 1), ImVec2(1, 0));
	}
}