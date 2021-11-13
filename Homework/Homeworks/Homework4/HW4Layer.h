#pragma once

#include <Editor/application.h>

#include <Renderer/texture.h>

#include <Image.h>

#include <imgui.h>

namespace Chaf
{
	class HW4Layer :public Layer
	{
	public:
		HW4Layer();
		virtual ~HW4Layer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(Timestep timestep) override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& event) override;

	private:
		void show_texture(Ref<Image>& image, const char* label, float width = ImGui::GetWindowContentRegionWidth() / 2);

	private:
		Ref<Image> m_src_image;
		Ref<Image> m_downsampled_image;
		Ref<Image> m_upsampled_image;
		Ref<Image> m_diff_image;

		float m_ssim = 0.f;
		float m_psnr = 0.f;

		bool m_is_dragging = false;

		bool m_updated = false;
	};
}