#include "HW1Layer.h"
#include <Base.h>

#include "ColorTransfer.h"
#include "Colorization.h"

#include <Editor/basic.h>

namespace Chaf
{
	HW1Layer::HW1Layer()
	{
	}

	void HW1Layer::OnAttach()
	{
	}

	void HW1Layer::OnDetach()
	{
	}

	void HW1Layer::OnUpdate(Timestep timestep)
	{
	}

	void HW1Layer::OnImGuiRender()
	{
		EditorBasic::GetFileDialog("Select Source Image", ".png,.jpg,.bmp,.jpeg", [this](const std::string& path) {
			if (!m_source)
			{
				m_source = CreateRef<Image>(path);
				m_result.reset();
				m_result = nullptr;
			}
			else if (m_source->getPath() != path)
			{
				m_source.reset();
				m_source = CreateRef<Image>(path);
				m_result.reset();
				m_result = nullptr;
			}
			});

		EditorBasic::GetFileDialog("Select Target Image", ".png,.jpg,.bmp,.jpeg", [this](const std::string& path) {
			if (!m_target)
			{
				m_target = CreateRef<Image>(path);
				m_result.reset();
				m_result = nullptr;
			}
			else if (m_target->getPath() != path)
			{
				m_target.reset();
				m_target = CreateRef<Image>(path);
				m_result.reset();
				m_result = nullptr;
			}
			});

		ImGui::Begin("Homework1");
		ImGui::Columns(3, "Homework1");

		ImGui::Text("Source Image");
		ImGui::PushID(0);
		if (ImGui::Button("Load"))
		{
			EditorBasic::SetPopupFlag("Select Source Image");
		}
		ImGui::SameLine();
		if (ImGui::Button("Clear"))
		{
			m_source.reset();
		}
		ImGui::SameLine();
		if (ImGui::Button("Save") && m_source)
		{
			m_source->save("source.png");
		}
		ImGui::PopID();
		show_texture(m_source, "Source");
		ImGui::NextColumn();

		ImGui::Text("Target Image");
		ImGui::PushID(1);
		if (ImGui::Button("Load"))
		{
			EditorBasic::SetPopupFlag("Select Target Image");
		}
		ImGui::SameLine();
		if (ImGui::Button("Clear"))
		{
			m_target.reset();
		}
		ImGui::SameLine();
		if (ImGui::Button("Save") && m_target)
		{
			m_target->save("target.png");
		}
		ImGui::PopID();
		show_texture(m_target, "Target");
		ImGui::NextColumn();

		ImGui::Text("Result");
		ImGui::PushID(2);
		if (ImGui::Button("Color Transfer") && m_source && m_target)
		{
			m_result.reset();
			ColorTransfer transfer(m_source, m_target);
			m_result = transfer.solve();
		}
		ImGui::SameLine();
		if (ImGui::Button("Colorization") && m_source && m_target)
		{
			m_result.reset();
			Colorization colorization(m_source, m_target);
			colorization.setMethod(ColorizationMethod::Global);
			m_result = colorization.solve();
		}
		ImGui::SameLine();
		if (ImGui::Button("Save") && m_result)
		{
			m_result->save("result.png");
		}
		ImGui::PopID();
		show_texture(m_result, "Result");

		ImGui::End();
	}

	void HW1Layer::OnEvent(Event& event)
	{
	}

	void HW1Layer::show_texture(Ref<Image>& image, const char* label)
	{
		if (!image)
		{
			return;
		}

		float width = ImGui::GetWindowContentRegionWidth() / 3;
		float height = width * static_cast<float>(image->getHeight()) / static_cast<float>(image->getWidth());

		ImGui::Image((void*)image->getTextureID(), { width, height }, ImVec2(0, 1), ImVec2(1, 0));
	}
}