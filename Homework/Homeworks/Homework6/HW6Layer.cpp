#include "HW6Layer.h"

#include <Editor/basic.h>

#include <imgui.h>
#include <implot.h>

#include "NSFW.h"

namespace Chaf
{
	HW6Layer::HW6Layer()
	{

	}

	void HW6Layer::OnAttach()
	{
	}

	void HW6Layer::OnDetach()
	{
	}

	void HW6Layer::OnUpdate(Timestep timestep)
	{
	}

	void HW6Layer::OnImGuiRender()
	{
		EditorBasic::GetFileDialog("Homework6##Select Source Image", ".png,.jpg,.bmp,.jpeg", [this](const std::string& path) {
			m_src_image = CreateRef<Image>(path);
			result = NSFW::check(m_src_image->getImage());
			});

		ImGui::Begin("Homework6");

		if (ImGui::Button("Load Source"))
		{
			EditorBasic::SetPopupFlag("Homework6##Select Source Image");
		}

		if (m_src_image)
		{
			ImGui::Text("classified result: %s", result.c_str());
			show_texture(m_src_image, "image", ImGui::GetWindowContentRegionWidth() * 2 / 3);
		}

		ImGui::End();
	}

	void HW6Layer::OnEvent(Event& event)
	{
	}

	void HW6Layer::show_texture(Ref<Image>& image, const char* label, float width)
	{
		if (!image)
		{
			return;
		}

		ImGui::Image((void*)image->getTextureID(), { width, width * static_cast<float>(image->getHeight()) / static_cast<float>(image->getWidth()) }, ImVec2(0, 1), ImVec2(1, 0));
	}
}