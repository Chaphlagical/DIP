#pragma once

#include <Editor/application.h>

#include <Renderer/texture.h>

#include <Image.h>

#include <imgui.h>

namespace Chaf
{
	class HW5Layer :public Layer
	{
	public:
		HW5Layer();
		virtual ~HW5Layer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(Timestep timestep) override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& event) override;

	private:
		void show_texture(Ref<Image>& image, const char* label, float width = ImGui::GetWindowContentRegionWidth() / 2);

		bool checkCurrentPoint();

	private:
		Ref<Image> m_src_image = nullptr;
		Ref<Image> m_dst_image = nullptr;
		Ref<Image> m_energy_map = nullptr;
		Ref<Image> m_seam_map = nullptr;
		Ref<Image> m_mask_map = nullptr;
		Ref<Image> m_contour_map = nullptr;
		Ref<Image> m_grabcut_result = nullptr;

		int m_Delta[2] = { 0, 0 };

		cv::Point2d m_current_point;

		bool m_updated = false;
		bool m_resized = false;
		bool m_dragging = false;

		std::vector<cv::Point2i>  m_forehead_points;
		std::vector<cv::Point2i>  m_background_points;

		std::vector<std::vector<cv::Point2d>>  m_forehead_draw_points;
		std::vector<std::vector<cv::Point2d>>  m_background_draw_points;
	};
}