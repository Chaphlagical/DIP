#pragma once

#include <Editor/application.h>

#include <Renderer/texture.h>

#include <Image.h>

#include <imgui.h>

namespace Chaf
{
	class HW3Layer :public Layer
	{
	public:
		HW3Layer();
		virtual ~HW3Layer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(Timestep timestep) override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& event) override;

	private:
		void show_texture(Ref<Image>& image, const char* label);

		bool checkCurrentPoint();

	private:
		Ref<Image> m_src_image;
		Ref<Image> m_tar_image;

		cv::Point2d m_current_point = {};

		bool m_is_dragging = false;

		bool m_updated = false;
	};
}