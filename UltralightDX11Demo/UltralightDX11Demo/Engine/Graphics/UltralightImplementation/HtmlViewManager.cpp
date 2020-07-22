#include "HtmlViewManager.h"

bool HtmlViewManager::Initialize(D3DClass* pd3d_)
{
	pd3d = pd3d_;

	//Initialize shared vertex buffer
	std::vector<Vertex_3pf_2tf> vertices =
	{
		Vertex_3pf_2tf(0.0f, 0.0f, 0.0f, 0.0f, 0.0f), //Bottom Left 
		Vertex_3pf_2tf(0.0f, 1.0f, 0.0f, 0.0f, 1.0f), //Top Left
		Vertex_3pf_2tf(1.0f, 1.0f, 0.0f, 1.0f, 1.0f), //Top Right

		Vertex_3pf_2tf(0.0f, 0.0f, 0.0f, 0.0f, 0.0f), //Bottom Left 
		Vertex_3pf_2tf(1.0f, 0.5f, 0.0f, 1.0f, 1.0f), //Top Right
		Vertex_3pf_2tf(1.0f, 0.0f, 0.0f, 1.0f, 0.0f), //Bottom Right
	};

	HRESULT hr = vertexBuffer.Initialize(pd3d->device.Get(), vertices);
	ReturnFalseIfFail(hr, "Renderer failed to initialize shared vertex buffer for view quad.");

	//Ultralight instance initialization
	ul::Config config;
	config.resource_path = ul::String16(std::string(DirectoryHelper::GetExecutableDirectoryA() + "resources").c_str());
	config.face_winding = ul::kFaceWinding_Clockwise;
	ul::Platform::instance().set_config(config);

	logger = std::make_unique<LoggerDefault>();
	ul::Platform::instance().set_logger(logger.get());

	fontLoader = std::make_unique<FontLoaderWin>();
	ul::Platform::instance().set_font_loader(fontLoader.get());

	fileSystem = std::make_unique<FileSystemDefault>();
	ul::Platform::instance().set_file_system(fileSystem.get());

	gpuDriver = std::make_unique<GPUDriverD3D11>(pd3d);
	ul::Platform::instance().set_gpu_driver(gpuDriver.get());

	ultralightRenderer = ul::Renderer::Create();
	if (ultralightRenderer.get() == nullptr)
		return false;

	return true;
}

void HtmlViewManager::UpdateViews()
{
	ultralightRenderer->Update();
	ultralightRenderer->Render();
	gpuDriver->DrawCommandList();
}

std::shared_ptr<HtmlView> HtmlViewManager::CreateView(uint32_t width_, uint32_t height_, bool isTransparent_)
{
	ul::RefPtr<ul::View> newView;
	newView = ultralightRenderer->CreateView(width_, height_, isTransparent_, nullptr);

	if (newView.get() == nullptr)
		return nullptr;

	std::shared_ptr<HtmlView> htmlView = std::make_shared<HtmlView>(gpuDriver.get(), newView, vertexBuffer, width_, height_, isTransparent_);
	htmlViews.push_back(htmlView);
	return htmlView;
}

void HtmlViewManager::FireMouseEvent(MouseEvent mouseEvent_)
{
	for (auto v : htmlViews)
	{
		v->FireMouseEvent(mouseEvent_.ToUltralightMouseEvent());
	}
}

void HtmlViewManager::FireKeyboardEvent(KeyboardEvent keyboardEvent_)
{
	for (auto v : htmlViews)
	{
		v->FireKeyboardEvent(keyboardEvent_.ToUltralightKeyboardEvent());
	}
}

std::vector<std::shared_ptr<HtmlView>>& HtmlViewManager::GetViews()
{
	return htmlViews;
}

HtmlViewManager::~HtmlViewManager()
{
	ultralightRenderer = nullptr;
}
