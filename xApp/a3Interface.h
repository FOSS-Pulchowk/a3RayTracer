#pragma once
#include "Common/Core.h"
#include "Platform/Platform.h"
#include "Platform/GL/LoadOpenGL.h"

#include "Platform/GL/GLDebug.h"
#include "Platform/GlRenderer.h"
#include "Platform/AssetManager.h"

#include "Math/Math.h"
#include "Math/Color.h"
#include "Platform/UIContext.h"

#include "Platform/GLResources.h"
#include "Utility/Resource.h"

#include <string>
#include <vector>
using namespace std;

#define ADD_COMPONENT(x) struct x { string name; }

static const v3 WhiteColorArray[4] = {
	{1.0f, 1.0f, 1.0f},
	{1.0f, 1.0f, 1.0f},
	{1.0f, 1.0f, 1.0f},
	{1.0f, 1.0f, 1.0f}
};

class App
{
private:
	enum pc_components
	{
		None = 0,
		Motherboard,
		Processor,
		Memory,
		Cooler,
		Storage,
		VideoCard,
		PowerSupply,
		Case,
		Total
	};
	ADD_COMPONENT(motherboard);
	ADD_COMPONENT(processor);
	ADD_COMPONENT(memory);
	ADD_COMPONENT(cooler);
	ADD_COMPONENT(storage);
	ADD_COMPONENT(video_card);
	ADD_COMPONENT(power_supply);
	ADD_COMPONENT(pc_case);

	a3::ui_context uiContext;
	pc_components currentlyOpen;
	a3::font_renderer fontRenderer;
	a3::basic2d_renderer renderer2D;

	vector<motherboard> vMotherBoards;
	vector<processor> vProcessors;
	vector<memory> vMemories;
	vector<cooler> vCoolers;
	vector<storage> vStorages;
	vector<video_card> vVideoCards;
	vector<power_supply> vPowerSupplies;
	vector<pc_case> vCases;

	static const char* s_ComponentNames[pc_components::Total - 1];

public:
	App();
	void Init();
	void Update(const a3::input_info& input);
	void Render();

private:
	void RenderUI(v2 position, v2 dimension);
	void RenderComponentWindow(pc_components c);
};

const char* App::s_ComponentNames[App::pc_components::Total - 1] =
{
	"Motherboard", "Processor", "Memory", "Cooler", "Storage", "Video Card", "Power Supply", "Case"
};

App::App() :
	uiContext(1280.0f, 720.0f),
	fontRenderer(a3::Renderer.CreateFontRenderer(a3::Shaders::GLFontVertex, a3::Shaders::GLFontFragment)),
	renderer2D(a3::Renderer.Create2DRenderer(a3::Shaders::GLBasic2DVertex, a3::Shaders::GLBasic2DFragment))
{
}

void App::Init()
{
	a3::Asset.LoadFontTextureAtlasFromFile(a3::asset_id::UserFont, "Resources/HackRegular.ttf", 100.0f);
	a3::Asset.LoadTexture2DFromFile(12, "Resources/BigSmile.png", GL_LINEAR, GL_CLAMP_TO_EDGE);
	a3::Asset.LoadTexture2DFromFile(12 + pc_components::Motherboard, "Resources/MotherBoard.png", GL_LINEAR, GL_CLAMP_TO_EDGE);
	a3::Asset.LoadTexture2DFromFile(12 + pc_components::Processor, "Resources/Processor.png", GL_LINEAR, GL_CLAMP_TO_EDGE);
	a3::Asset.LoadTexture2DFromFile(12 + pc_components::Memory, "Resources/Memory.png", GL_LINEAR, GL_CLAMP_TO_EDGE);
	a3::Asset.LoadTexture2DFromFile(12 + pc_components::Storage, "Resources/Storage.png", GL_LINEAR, GL_CLAMP_TO_EDGE);
	a3::Asset.LoadTexture2DFromFile(12 + pc_components::Case, "Resources/Case.png", GL_LINEAR, GL_CLAMP_TO_EDGE);
	a3::Asset.LoadTexture2DFromFile(12 + pc_components::Cooler, "Resources/Cooler.png", GL_LINEAR, GL_CLAMP_TO_EDGE);
	a3::Asset.LoadTexture2DFromFile(12 + pc_components::PowerSupply, "Resources/PowerSupply.png", GL_LINEAR, GL_CLAMP_TO_EDGE);
	a3::Asset.LoadTexture2DFromFile(12 + pc_components::VideoCard, "Resources/VideoCard.png", GL_LINEAR, GL_CLAMP_TO_EDGE);

	currentlyOpen = pc_components::None;
	uiContext.SetColor(a3::color::Aqua, a3::color::Aqua, a3::color::Blurple, a3::color::WhiteSmoke);
	fontRenderer.SetRegion(0.0f, 1280.0f, 0.0f, 720.0f);
	fontRenderer.SetFont(a3::Asset.Get<a3::font_texture>(a3::asset_id::UserFont));


	renderer2D.SetRegion(0.0f, 1280.0f, 0.0f, 720.0f);
}

void App::Update(const a3::input_info& input)
{
	uiContext.UpdateIO(input);
}

void App::Render()
{
	renderer2D.BeginFrame();
	renderer2D.Push({ 50.0f, 50.0f }, { 500.0f, 500.0f }, WhiteColorArray, a3::Asset.Get<a3::texture>(12 + currentlyOpen));
	renderer2D.EndFrame();


	fontRenderer.Render("PC Parts", { 1050.0f, 670.0f }, 45.0f, a3::color::White);
	RenderUI({ 1050.0f, 600.0f }, { 200.0f, 50.0f });
	RenderComponentWindow(currentlyOpen);
}

void App::RenderUI(v2 position, v2 dimension)
{
	for (i32 i = 1; i < pc_components::Total; ++i)
	{
		if (uiContext.Button(i, position, dimension, s_ComponentNames[i - 1]))
		{
			if (currentlyOpen == (pc_components)i)
				currentlyOpen = pc_components::None;
			else
				currentlyOpen = (pc_components)i;
		}
		position.y -= dimension.y;
	}
}

void App::RenderComponentWindow(App::pc_components c)
{
	switch (c)
	{
	case pc_components::Motherboard:
	{

		break;
	}
	case pc_components::Processor:
	{

		break;
	}
	case pc_components::Memory:
	{

		break;
	}
	case pc_components::Cooler:
	{

		break;
	}
	case pc_components::Storage:
	{

		break;
	}
	case pc_components::VideoCard:
	{

		break;
	}
	case pc_components::PowerSupply:
	{

		break;
	}
	case pc_components::Case:
	{

		break;
	}
	}
}