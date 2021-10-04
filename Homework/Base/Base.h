#pragma once

#include <Editor/application.h>
#include <Editor/editor_layer.h>
#include <Scene/maincamera_layer.h>
#include <Scene/scene_layer.h>

namespace Chaf
{
	class App : public Application
	{
	public:
		App()
			:Application("Digital Image Process Homework")
		{
		}

		~App()
		{

		}
	};

	Application* Create()
	{
		return new App();
	}
}