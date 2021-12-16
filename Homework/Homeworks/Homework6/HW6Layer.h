#pragma once

#include <Editor/application.h>

#include <Renderer/texture.h>

#include <Image.h>

#include <imgui.h>

namespace Chaf
{
	class HW6Layer :public Layer
	{
	public:
		HW6Layer();
		virtual ~HW6Layer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(Timestep timestep) override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& event) override;

	private:
		void show_texture(Ref<Image>& image, const char* label, float width = ImGui::GetWindowContentRegionWidth() / 2);

	private:
		Ref<Image> m_src_image = nullptr;
		std::string result;
	};
}