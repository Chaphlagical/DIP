#pragma once

#include <Editor/application.h>

#include <Renderer/texture.h>

#include <Image.h>

#include <imgui.h>

#include "Warping.h"

namespace Chaf
{
	class HW2Layer :public Layer
	{
	public:
		HW2Layer();
		virtual ~HW2Layer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(Timestep timestep) override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& event) override;

	private:
		void show_texture(Ref<Image>& image, const char* label);

		bool checkCurrentPoint();

	private:
		Ref<Image> m_image;
		Ref<Image> m_result;

		Scope<Warping> m_solver;

		cv::Mat mask;

		std::vector<cv::Point2d> m_src_points;
		std::vector<cv::Point2d> m_dst_points;

		bool is_idw = false;

		cv::Point2d m_current_point = {};

		bool m_is_dragging = false;

		bool m_updated = false;
	};
}