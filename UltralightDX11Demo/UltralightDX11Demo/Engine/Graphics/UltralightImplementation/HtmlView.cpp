#include "HtmlView.h"

HtmlView::HtmlView(GPUDriverD3D11* pGpuDriver_, ul::RefPtr<ul::View> view_, VertexBuffer<Vertex_3pf_2tf>& vertexBuffer_, uint32_t width_, uint32_t height_, bool isTransparent_)
	:vertexBuffer(vertexBuffer_)
{
	pGpuDriver = pGpuDriver_;
	view = view_;
	width = width_;
	height = height_;
	isTransparent = isTransparent_;
	/*viewLoadListener = std::make_unique<HtmlViewLoadListener>();
	view->set_load_listener(viewLoadListener.get());
	viewViewListener = std::make_unique<HtmlViewViewListener>();
	view->set_view_listener(viewViewListener.get());*/
	UpdateWorldMatrix();
}

void HtmlView::LoadURL(std::string url_)
{
	view->LoadURL(ul::String(url_.c_str()));
}

bool HtmlView::IsLoading()
{
	return view->is_loading();
}

void HtmlView::FireMouseEvent(ul::MouseEvent mouseEvent_)
{
	mouseEvent_.x -= position.x;
	mouseEvent_.y -= position.y;
	view->FireMouseEvent(mouseEvent_);
}

void HtmlView::FireKeyboardEvent(ul::KeyEvent keyboardEvent_)
{
	view->FireKeyEvent(keyboardEvent_);
}

void HtmlView::Focus()
{
	view->Focus();
}

void HtmlView::SetSize(uint32_t width_, uint32_t height_)
{
	width = width_;
	height = height_;
	view->Resize(width, height);
	UpdateWorldMatrix();
}

void HtmlView::SetPosition(float x_, float y_)
{
	position.x = x_;
	position.y = y_;
	UpdateWorldMatrix();
}

DirectX::XMMATRIX HtmlView::GetWorldMatrix()
{
	return worldMatrix;
}

ID3D11ShaderResourceView* const* HtmlView::GetAddressOfShaderResourceView()
{
	return pGpuDriver->GetAddressOfShaderResourceView(view);
}

VertexBuffer<Vertex_3pf_2tf>* HtmlView::GetVertexBuffer()
{
	return &vertexBuffer;
}

HtmlView::~HtmlView()
{
	view = nullptr;
}

void HtmlView::UpdateWorldMatrix()
{
	worldMatrix = DirectX::XMMatrixScaling(width, height, 1) * DirectX::XMMatrixTranslation(position.x, position.y, 0);
}
