#include "HW3Layer.h"

#include <Editor/basic.h>

#include <imgui.h>
#include <implot.h>

namespace Chaf
{
	HW3Layer::HW3Layer()
	{

	}

	void HW3Layer::OnAttach()
	{
	}

	void HW3Layer::OnDetach()
	{
	}

	void HW3Layer::OnUpdate(Timestep timestep)
	{

	}
	void HW3Layer::OnImGuiRender()
	{
		EditorBasic::GetFileDialog("Homework3##Select Source Image", ".png,.jpg,.bmp,.jpeg", [this](const std::string& path) {

				m_src_image = CreateRef<Image>(path);
			
			});

		EditorBasic::GetFileDialog("Homework3##Select Target Image", ".png,.jpg,.bmp,.jpeg", [this](const std::string& path) {

				m_tar_image = CreateRef<Image>(path);
			
			});

		ImGui::Begin("Homework3");
		ImGui::Columns(2, "Homework3");

		if (ImGui::Button("Load Source"))
		{
			EditorBasic::SetPopupFlag("Homework3##Select Source Image");
		}

		show_texture(m_src_image, "image");

		auto draw_list = ImGui::GetWindowDrawList();

		m_current_point = cv::Point2d{ static_cast<double>((ImGui::GetMousePos().x - ImGui::GetItemRectMin().x) / ImGui::GetItemRectSize().x), static_cast<double>((ImGui::GetMousePos().y - ImGui::GetItemRectMin().y) / ImGui::GetItemRectSize().y) };

		ImGui::NextColumn();

		if (ImGui::Button("Load Target"))
		{
			EditorBasic::SetPopupFlag("Homework3##Select Target Image");
		}

		show_texture(m_tar_image, "result");

		ImGui::End();
	}

	void HW3Layer::OnEvent(Event& event)
	{
	}

	void HW3Layer::show_texture(Ref<Image>& image, const char* label)
	{
		if (!image)
		{
			return;
		}

		auto width = ImGui::GetWindowContentRegionWidth() / 2;

		ImGui::Image((void*)image->getTextureID(), { width, width * static_cast<float>(image->getHeight()) / static_cast<float>(image->getWidth()) }, ImVec2(0, 1), ImVec2(1, 0));
	}

	bool HW3Layer::checkCurrentPoint()
	{
		if (m_src_image)
		{
			return m_current_point.x >= 0 && m_current_point.y >= 0 && m_current_point.x < 1.0 && m_current_point.y < 1.0;
		}
		return false;
	}
}