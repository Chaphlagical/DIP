#include "HW5Layer.h"

#include <Editor/basic.h>

#include <imgui.h>
#include <implot.h>

#include "SeamCarving.h"

#include "LazySnapping.h"

namespace Chaf
{
	HW5Layer::HW5Layer()
	{

	}

	void HW5Layer::OnAttach()
	{
	}

	void HW5Layer::OnDetach()
	{
	}

	void HW5Layer::OnUpdate(Timestep timestep)
	{
	}

	void HW5Layer::OnImGuiRender()
	{
		EditorBasic::GetFileDialog("Homework5##Select Source Image", ".png,.jpg,.bmp,.jpeg", [this](const std::string& path) {
			m_src_image = CreateRef<Image>(path);
			m_dst_image = CreateRef<Image>(path);
			});

		ImGui::Begin("Homework5");
		ImGui::Columns(2, "Homework5");

		if (ImGui::Button("Load Source"))
		{
			EditorBasic::SetPopupFlag("Homework5##Select Source Image");
		}

		if (ImGui::Button("Clear"))
		{
			m_forehead_points.clear();
			m_background_points.clear();
			m_forehead_draw_points.clear();
			m_background_draw_points.clear();
		}

		if (ImGui::Button("Save"))
		{
			if (m_src_image)
				m_src_image->save("m_src_image.png");
			if (m_dst_image)
				m_dst_image->save("m_dst_image.png");
			if (m_energy_map)
				m_energy_map->save("m_energy_map.png");
			if (m_seam_map)
				m_seam_map->save("m_seam_map.png");
			if (m_mask_map)
				m_mask_map->save("m_mask_map.png");
			if (m_contour_map)
				m_contour_map->save("m_contour_map.png");
		}

		if (!m_updated && (m_background_points.size() > 0 || m_forehead_points.size() > 0))
		{
			LazySnapping solver;
			cv::Mat src_image = m_src_image->getImage();
			cv::resize(src_image, src_image, cv::Size(src_image.cols / 8, src_image.rows / 8));
			solver.setImage(src_image);
			solver.setForeheadPoints(m_forehead_points);
			solver.setBackgroundPoints(m_background_points);
			solver.runMaxflow();
			cv::Mat mask = solver.getMask();
			cv::resize(mask, mask, cv::Size(mask.cols * 8, mask.rows * 8));
			m_mask_map = CreateRef<Image>(mask);
			m_contour_map = CreateRef<Image>(solver.drawContours(m_src_image->getImage(), m_mask_map->getImage()));
		}

		show_texture(m_src_image, "image", ImGui::GetWindowContentRegionWidth() / 3.f);

		auto draw_list = ImGui::GetWindowDrawList();

		for (auto& point_list : m_forehead_draw_points)
		{
			for (int i = 0; i < static_cast<int>(point_list.size()) - 1; i++)
			{
				draw_list->AddLine({ static_cast<float>(point_list[i].x) * ImGui::GetItemRectSize().x + ImGui::GetItemRectMin().x ,static_cast<float>(point_list[i].y) * ImGui::GetItemRectSize().y + ImGui::GetItemRectMin().y },
					{ static_cast<float>(point_list[i + 1].x) * ImGui::GetItemRectSize().x + ImGui::GetItemRectMin().x ,static_cast<float>(point_list[i + 1].y) * ImGui::GetItemRectSize().y + ImGui::GetItemRectMin().y }, ImColor(0, 0, 255), 4.0);
			}
		}

		for (auto& point_list : m_background_draw_points)
		{
			for (int i = 0; i < static_cast<int>(point_list.size()) - 1; i++)
			{
				draw_list->AddLine({ static_cast<float>(point_list[i].x) * ImGui::GetItemRectSize().x + ImGui::GetItemRectMin().x ,static_cast<float>(point_list[i].y) * ImGui::GetItemRectSize().y + ImGui::GetItemRectMin().y },
					{ static_cast<float>(point_list[i + 1].x) * ImGui::GetItemRectSize().x + ImGui::GetItemRectMin().x ,static_cast<float>(point_list[i + 1].y) * ImGui::GetItemRectSize().y + ImGui::GetItemRectMin().y }, ImColor(255, 0, 0), 4.0);
			}
		}

		m_current_point = cv::Point2d{ static_cast<double>((ImGui::GetMousePos().x - ImGui::GetItemRectMin().x) / ImGui::GetItemRectSize().x), static_cast<double>((ImGui::GetMousePos().y - ImGui::GetItemRectMin().y) / ImGui::GetItemRectSize().y) };

		if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && checkCurrentPoint())
		{
			m_forehead_draw_points.push_back(std::vector<cv::Point2d>());
			m_forehead_points.push_back(cv::Point2i(int(m_current_point.x * m_src_image->getWidth() / 8), int(m_current_point.y * m_src_image->getHeight() / 8)));
			m_forehead_draw_points.back().push_back(m_current_point);
			m_updated = true;
		}

		if (ImGui::IsWindowHovered() && ImGui::IsMouseDragging(ImGuiMouseButton_Left) && checkCurrentPoint())
		{
			if (m_forehead_draw_points.empty())
			{
				m_forehead_draw_points.push_back(std::vector<cv::Point2d>());
			}
			m_forehead_points.push_back(cv::Point2i(int(m_current_point.x * m_src_image->getWidth() / 8), int(m_current_point.y * m_src_image->getHeight() / 8)));
			m_forehead_draw_points.back().push_back(m_current_point);
			m_updated = true;
		}

		if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right) && checkCurrentPoint())
		{
			m_background_draw_points.push_back(std::vector<cv::Point2d>());
			m_background_points.push_back(cv::Point2i(int(m_current_point.x * m_src_image->getWidth() / 8), int(m_current_point.y * m_src_image->getHeight() / 8)));
			m_background_draw_points.back().push_back(m_current_point);
			m_updated = true;
		}

		if (ImGui::IsWindowHovered() && ImGui::IsMouseDragging(ImGuiMouseButton_Right) && checkCurrentPoint())
		{
			if (m_background_draw_points.empty())
			{
				m_background_draw_points.push_back(std::vector<cv::Point2d>());
			}
			m_background_points.push_back(cv::Point2i(int(m_current_point.x * m_src_image->getWidth() / 8), int(m_current_point.y * m_src_image->getHeight() / 8)));
			m_background_draw_points.back().push_back(m_current_point);
			m_updated = true;
		}

		ImGui::NextColumn();

		if (m_src_image)
		{
			ImGui::Text("Original: %dx%d", m_src_image->getWidth(), m_src_image->getHeight());
			ImGui::Text("Resized: %dx%d", m_dst_image->getWidth(), m_dst_image->getHeight());
			if (!m_updated)
			{
				ImGui::DragInt("Delta X", &m_Delta[0], 1, -static_cast<int>(m_dst_image->getWidth()), m_dst_image->getWidth());
				ImGui::DragInt("Delta Y", &m_Delta[1], 1, -static_cast<int>(m_dst_image->getHeight()), m_dst_image->getHeight());
			}
			else
			{
				ImGui::Text("Delta X: %d", m_Delta[0]);
				ImGui::Text("Delta Y: %d", m_Delta[1]);
			}
		}

		if (ImGui::Button("Update"))
		{
			m_updated = true;
		}

		if (m_Delta[0] == 0 && m_Delta[1] == 0)
		{
			m_updated = false;
		}

		// Only narrow width
		if (m_dst_image && m_Delta[0] < 0 && m_Delta[1] >= 0 && m_updated)
		{
			++m_Delta[0];

			cv::Mat src_img = m_dst_image->getImage();
			// Generate energy map
			cv::Mat energy_map;
			SeamCarving::genEnergyMap(src_img, energy_map);

			// Find seam
			std::vector<int> seam;
			SeamCarving::findSeamVertical(energy_map, seam);

			// display energy map
			energy_map.convertTo(energy_map, CV_8UC3);
			cv::cvtColor(energy_map, energy_map, cv::COLOR_GRAY2BGR);
			m_energy_map = CreateRef<Image>(energy_map);

			for (int i = 0; i < seam.size(); i++)
			{
				src_img.at<cv::Vec3b>(i, seam[i]) = cv::Vec3b(0, 0, 255);
			}
			m_seam_map = CreateRef<Image>(src_img);

			cv::Mat dst_img;
			SeamCarving::narrowSeamVertical(src_img, dst_img, seam);
			m_dst_image = CreateRef<Image>(dst_img);
		}

		// Only narrow height
		if (m_dst_image && m_Delta[1] < 0 && m_Delta[0] >= 0 && m_updated)
		{
			++m_Delta[1];

			cv::Mat src_img = m_dst_image->getImage();
			// Generate energy map
			cv::Mat energy_map;
			SeamCarving::genEnergyMap(src_img, energy_map);

			// Find seam
			std::vector<int> seam;
			SeamCarving::findSeamHorizontal(energy_map, seam);

			// display energy map
			energy_map.convertTo(energy_map, CV_8UC3);
			cv::cvtColor(energy_map, energy_map, cv::COLOR_GRAY2BGR);
			m_energy_map = CreateRef<Image>(energy_map);

			for (int i = 0; i < seam.size(); i++)
			{
				src_img.at<cv::Vec3b>(seam[i], i) = cv::Vec3b(0, 0, 255);
			}
			m_seam_map = CreateRef<Image>(src_img);

			cv::Mat dst_img;
			SeamCarving::narrowSeamHorizontal(src_img, dst_img, seam);
			m_dst_image = CreateRef<Image>(dst_img);
		}

		// Both direction narrow
		if (m_dst_image && m_Delta[0] < 0 && m_Delta[1] < 0 && m_updated)
		{
			cv::Mat src_img = m_dst_image->getImage();
			cv::Mat energy_map;
			std::vector<int> seam_x, seam_y;
			SeamCarving::genEnergyMap(src_img, energy_map);

			float Ex = SeamCarving::findSeamVertical(energy_map, seam_x);
			float Ey = SeamCarving::findSeamHorizontal(energy_map, seam_y);

			// display energy map
			energy_map.convertTo(energy_map, CV_8UC3);
			cv::cvtColor(energy_map, energy_map, cv::COLOR_GRAY2BGR);
			m_energy_map = CreateRef<Image>(energy_map);

			if (Ex < Ey)
			{
				for (int i = 0; i < seam_x.size(); i++)
				{
					src_img.at<cv::Vec3b>(i, seam_x[i]) = cv::Vec3b(0, 0, 255);
				}
				m_seam_map = CreateRef<Image>(src_img);

				cv::Mat dst_img;
				SeamCarving::narrowSeamVertical(src_img, dst_img, seam_x);
				m_dst_image = CreateRef<Image>(dst_img);
				m_Delta[0]++;
			}
			else
			{
				for (int i = 0; i < seam_y.size(); i++)
				{
					src_img.at<cv::Vec3b>(seam_y[i], i) = cv::Vec3b(0, 0, 255);
				}
				m_seam_map = CreateRef<Image>(src_img);

				cv::Mat dst_img;
				SeamCarving::narrowSeamHorizontal(src_img, dst_img, seam_y);
				m_dst_image = CreateRef<Image>(dst_img);
				m_Delta[1]++;
			}
		}

		// Only enlarge width
		if (m_dst_image && m_Delta[0] > 0 && m_Delta[1] == 0 && m_updated)
		{
			cv::Mat src_img = m_dst_image->getImage();
			// Generate energy map
			cv::Mat energy_map;
			SeamCarving::genEnergyMap(src_img, energy_map);

			// Find seam
			std::vector<std::vector<int>> seams;
			SeamCarving::findSeamVertical(energy_map, seams, m_Delta[0]);

			// display energy map
			energy_map.convertTo(energy_map, CV_8UC3);
			cv::cvtColor(energy_map, energy_map, cv::COLOR_GRAY2BGR);
			m_energy_map = CreateRef<Image>(energy_map);

			cv::Mat dst_img;
			SeamCarving::enlargeSeamVertical(src_img, dst_img, seams);
			m_dst_image = CreateRef<Image>(dst_img);

			for (auto& seam : seams)
			{
				for (int i = 0; i < seam.size(); i++)
				{
					src_img.at<cv::Vec3b>(i, seam[i]) = cv::Vec3b(0, 0, 255);
				}
			}

			m_seam_map = CreateRef<Image>(src_img);

			m_updated = false;
			m_Delta[0] = 0;
		}

		// Only enlarge height
		if (m_dst_image && m_Delta[1] > 0 && m_Delta[0] == 0 && m_updated)
		{
			cv::Mat src_img = m_dst_image->getImage();
			// Generate energy map
			cv::Mat energy_map;
			SeamCarving::genEnergyMap(src_img, energy_map);

			// Find seam
			std::vector<std::vector<int>> seams;
			SeamCarving::findSeamHorizontal(energy_map, seams, m_Delta[1]);

			// display energy map
			energy_map.convertTo(energy_map, CV_8UC3);
			cv::cvtColor(energy_map, energy_map, cv::COLOR_GRAY2BGR);
			m_energy_map = CreateRef<Image>(energy_map);

			cv::Mat dst_img;
			SeamCarving::enlargeSeamHorizontal(src_img, dst_img, seams);
			m_dst_image = CreateRef<Image>(dst_img);

			for (auto& seam : seams)
			{
				for (int i = 0; i < seam.size(); i++)
				{
					src_img.at<cv::Vec3b>(seam[i], i) = cv::Vec3b(0, 0, 255);
				}
			}

			m_seam_map = CreateRef<Image>(src_img);

			m_updated = false;
			m_Delta[1] = 0;
		}

		if (m_dst_image && m_Delta[0] > 0 && m_Delta[1] > 0 && m_updated)
		{
			cv::Mat src_img = m_dst_image->getImage();
			// Generate energy map
			cv::Mat energy_map;
			SeamCarving::genEnergyMap(src_img, energy_map);

			// Find seam
			std::vector<std::vector<int>> seams_x, seams_y;
			SeamCarving::findSeamVertical(energy_map, seams_x, m_Delta[0]);
			SeamCarving::findSeamHorizontal(energy_map, seams_y, m_Delta[1]);

			// display energy map
			energy_map.convertTo(energy_map, CV_8UC3);
			cv::cvtColor(energy_map, energy_map, cv::COLOR_GRAY2BGR);
			m_energy_map = CreateRef<Image>(energy_map);

			cv::Mat dst_img;
			SeamCarving::enlargeSeamVertical(src_img, dst_img, seams_x);
			SeamCarving::enlargeSeamVertical(dst_img, dst_img, seams_y);
			m_dst_image = CreateRef<Image>(dst_img);

			for (auto& seam : seams_x)
			{
				for (int i = 0; i < seam.size(); i++)
				{
					src_img.at<cv::Vec3b>(i, seam[i]) = cv::Vec3b(0, 0, 255);
				}
			}

			for (auto& seam : seams_y)
			{
				for (int i = 0; i < seam.size(); i++)
				{
					src_img.at<cv::Vec3b>(seam[i], i) = cv::Vec3b(0, 0, 255);
				}
			}

			m_seam_map = CreateRef<Image>(src_img);

			m_updated = false;
			m_Delta[0] = 0;
			m_Delta[1] = 0;
		}

		if (m_mask_map)
		{
			ImGui::Text("Mask Map");
			show_texture(m_mask_map, "mask map", ImGui::GetWindowContentRegionWidth() / 3.f);
		}

		if (m_contour_map)
		{
			ImGui::Text("Contour Map");
			show_texture(m_contour_map, "contour map", ImGui::GetWindowContentRegionWidth() / 3.f);
		}

		if (m_energy_map)
		{
			ImGui::Text("Energy Map");
			show_texture(m_energy_map, "energy map", ImGui::GetWindowContentRegionWidth() / 3.f);
		}

		if (m_seam_map)
		{
			ImGui::Text("Seam Map");
			show_texture(m_seam_map, "seam map", ImGui::GetWindowContentRegionWidth() / 3.f);
		}

		if (m_dst_image)
		{
			ImGui::Text("Dst Map");
			show_texture(m_dst_image, "dst map", ImGui::GetWindowContentRegionWidth() / 3.f);
		}

		ImGui::End();
	}

	void HW5Layer::OnEvent(Event& event)
	{
	}

	void HW5Layer::show_texture(Ref<Image>& image, const char* label, float width)
	{
		if (!image)
		{
			return;
		}

		ImGui::Image((void*)image->getTextureID(), { static_cast<float>(image->getWidth()),  static_cast<float>(image->getHeight()) }, ImVec2(0, 1), ImVec2(1, 0));
	}

	bool HW5Layer::checkCurrentPoint()
	{
		if (m_src_image)
		{
			return m_current_point.x >= 0 && m_current_point.y >= 0 && m_current_point.x < 1.0 && m_current_point.y < 1.0;
		}
		return false;
	}
}