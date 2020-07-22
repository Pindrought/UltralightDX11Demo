#pragma once
#include "HtmlView.h"
#include "../../Window/Input/KeyboardEvent.h"
#include "../../Window/Input/MouseEvent.h"
#include "../D3D/D3DClass.h"
#include "../D3D/VertexBuffer.h"

#include "LoggerAdapters/LoggerDefault.h"
#include "FontLoaderAdapters/FontLoaderWin.h"
#include "FileSystems/FileSystemDefault.h"
#include "GPUAdapters/GPUDriverD3D11.h"

class HtmlViewManager
{
public:
	bool Initialize(D3DClass* pd3d);
	void UpdateViews();
	std::shared_ptr<HtmlView> CreateView(uint32_t width, uint32_t height, bool isTransparent = false);
	void FireMouseEvent(MouseEvent mouseEvent);
	void FireKeyboardEvent(KeyboardEvent keyboardEvent);
	std::vector<std::shared_ptr<HtmlView>>& GetViews();
	~HtmlViewManager();

private:
	D3DClass* pd3d = nullptr;
	ul::RefPtr<ul::Renderer> ultralightRenderer;
	std::unique_ptr<LoggerDefault> logger;
	std::unique_ptr<FontLoaderWin> fontLoader;
	std::unique_ptr<FileSystemDefault> fileSystem;
	std::unique_ptr<GPUDriverD3D11> gpuDriver;
	std::vector<std::shared_ptr<HtmlView>> htmlViews;
	VertexBuffer<Vertex_3pf_2tf> vertexBuffer;
};