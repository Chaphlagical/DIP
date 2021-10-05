#include "HW2Layer.h"
#include "WarpingIDW.h"
#include "WarpingRBF.h"

#include <Editor/basic.h>

#include <imgui.h>
#include <implot.h>

namespace Chaf
{
	HW2Layer::HW2Layer()
	{
		m_solver = CreateScope<WarpingRBF>();
	}

	void HW2Layer::OnAttach()
	{
	}

	void HW2Layer::OnDetach()
	{
	}

	void HW2Layer::OnUpdate(Timestep timestep)
	{
		if (m_image && m_src_points.empty() && m_updated)
		{
			m_result = CreateRef<Image>(m_image->getImage());
		}

		if (m_image && !m_src_points.empty() && m_updated)
		{
			mask = m_solver->warp(*m_image, *m_result, m_src_points, m_dst_points);
			m_result = CreateRef<Image>(m_result->getImage());
			m_updated = false;
		}
	}
	void HW2Layer::OnImGuiRender()
	{
		EditorBasic::GetFileDialog("Select Image", ".png,.jpg,.bmp,.jpeg", [this](const std::string& path) {
			if (!m_image)
			{
				m_image = CreateRef<Image>(path);
				m_result = CreateRef<Image>(m_image->getImage().clone());
			}
			else if (m_image->getPath() != path)
			{
				m_image.reset();
				m_image = CreateRef<Image>(path);
				m_result = CreateRef<Image>(m_image->getImage().clone());
			}
			});

		ImGui::Begin("Homework2");

		if (ImGui::Button("Load Image"))
		{
			EditorBasic::SetPopupFlag("Select Image");
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("IDW", is_idw))
		{
			is_idw = true;
			m_solver = CreateScope<WarpingIDW>();
			m_updated = true;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("RBF", !is_idw))
		{
			is_idw = false;
			m_solver = CreateScope<WarpingRBF>();
			m_updated = true;
		}

		ImGui::SameLine();
		if (ImGui::Button("Back"))
		{
			if (!m_src_points.empty())
			{
				m_src_points.pop_back();
				m_dst_points.pop_back();
				m_updated = true;
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Clear"))
		{
			m_src_points.clear();
			m_dst_points.clear();
			m_updated = true;
		}

		ImGui::SameLine();
		if (!mask.empty() && ImGui::Button("Fix"))
		{
			m_solver->fix(*m_result, mask);
			m_result = CreateRef<Image>(m_result->getImage());
		}
		ImGui::SameLine();
		if (m_result && ImGui::Button("Save"))
		{
			m_result->save("result.png");
		}

		ImGui::Columns(2, "Homework1");

		show_texture(m_image, "image");

		auto draw_list = ImGui::GetWindowDrawList();

		for (uint32_t i = 0; i < m_src_points.size(); i++)
		{
			draw_list->AddCircleFilled({ static_cast<float>(m_src_points[i].x) * ImGui::GetItemRectSize().x + ImGui::GetItemRectMin().x ,static_cast<float>(m_src_points[i].y) * ImGui::GetItemRectSize().y + ImGui::GetItemRectMin().y }, 8.0, ImColor(255, 0, 0));
			draw_list->AddCircleFilled({ static_cast<float>(m_dst_points[i].x) * ImGui::GetItemRectSize().x + ImGui::GetItemRectMin().x ,static_cast<float>(m_dst_points[i].y) * ImGui::GetItemRectSize().y + ImGui::GetItemRectMin().y }, 8.0, ImColor(0, 255, 0));
			draw_list->AddLine({ static_cast<float>(m_src_points[i].x) * ImGui::GetItemRectSize().x + ImGui::GetItemRectMin().x ,static_cast<float>(m_src_points[i].y) * ImGui::GetItemRectSize().y + ImGui::GetItemRectMin().y },
				{ static_cast<float>(m_dst_points[i].x) * ImGui::GetItemRectSize().x + ImGui::GetItemRectMin().x ,static_cast<float>(m_dst_points[i].y) * ImGui::GetItemRectSize().y + ImGui::GetItemRectMin().y }, ImColor(0, 0, 255), 4.0);
		}

		m_current_point = cv::Point2d{ static_cast<double>((ImGui::GetMousePos().x - ImGui::GetItemRectMin().x) / ImGui::GetItemRectSize().x), static_cast<double>((ImGui::GetMousePos().y - ImGui::GetItemRectMin().y) / ImGui::GetItemRectSize().y) };

		ImGui::NextColumn();

		show_texture(m_result, "result");

		if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsMouseDragging(ImGuiMouseButton_Left) && checkCurrentPoint())
		{
			m_src_points.push_back(m_current_point);
			m_dst_points.push_back(m_current_point);
		}
		else if (ImGui::IsWindowHovered() && ImGui::IsMouseDragging(ImGuiMouseButton_Left) && checkCurrentPoint())
		{
			m_updated = true;
			m_dst_points.back() = m_current_point;
			m_is_dragging = true;
		}
		else if (ImGui::IsWindowHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left) && checkCurrentPoint())
		{
			m_is_dragging = false;
			m_updated = true;
		}
		else
		{
			m_is_dragging = false;
		}

		ImGui::End();
	}

	void HW2Layer::OnEvent(Event& event)
	{
	}

	void HW2Layer::show_texture(Ref<Image>& image, const char* label)
	{
		if (!image)
		{
			return;
		}

		auto width = ImGui::GetWindowContentRegionWidth() / 2;

		ImGui::Image((void*)image->getTextureID(), { width, width * static_cast<float>(image->getHeight()) / static_cast<float>(image->getWidth()) }, ImVec2(0, 1), ImVec2(1, 0));
	}

	bool HW2Layer::checkCurrentPoint()
	{
		if (m_image)
		{
			return m_current_point.x >= 0 && m_current_point.y >= 0 && m_current_point.x < 1.0 && m_current_point.y < 1.0;
		}
		return false;
	}
}