#pragma once

#include <Editor/application.h>

#include <Renderer/texture.h>

#include <Image.h>

namespace Chaf
{
	class HW1Layer :public Layer
	{
	public:
		HW1Layer();
		virtual ~HW1Layer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(Timestep timestep) override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event & event) override;

	private:
		void show_texture(Ref<Image>& image, const char* label);

	private:
		Ref<Image> m_source;
		Ref<Image> m_target;
		Ref<Image> m_result;
	};
}