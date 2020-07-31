#include "HtmlView.h"

HtmlView::HtmlView(GPUDriverD3D11* pGpuDriver_, ul::RefPtr<ul::View> view_, VertexBuffer<Vertex_3pf_2tf>& vertexBuffer_, uint32_t width_, uint32_t height_, bool isTransparent_)
	:vertexBuffer(vertexBuffer_)
{
	pGpuDriver = pGpuDriver_;
	view = view_;
	width = width_;
	height = height_;
	isTransparent = isTransparent_;
	viewLoadListener = std::make_unique<HtmlViewLoadListener>();
	view->set_load_listener(viewLoadListener.get());

	viewViewListener = std::make_unique<HtmlViewViewListener>();
	view->set_view_listener(viewViewListener.get());

	RegisterNativeCFunctions();
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

JSValueRef NativeMessageBox(JSContextRef ctx_, 
							JSObjectRef fnc_, 
							JSObjectRef thisObject_, 
							size_t argCount_, 
							const JSValueRef args_[], 
							JSValueRef* exception_)
{
	if (argCount_ != 1)
	{
		OutputDebugStringA("NativeMessageBox improperly called in javascript. Expected exactly 1 argument of type string.");
		return JSValueMakeNull(ctx_);
	}

	JSType argType = JSValueGetType(ctx_, args_[0]);
	if (argType != JSType::kJSTypeString)
	{
		OutputDebugStringA("NativeMessageBox improperly called in javascript with an argument that was not of type string.");
		return JSValueMakeNull(ctx_);
	}

	JSStringRef msgArgumentJSRef = JSValueToStringCopy(ctx_, args_[0], NULL);
	size_t length = JSStringGetLength(msgArgumentJSRef) + 1;
	std::unique_ptr<char[]> stringBuffer = std::make_unique<char[]>(length);
	JSStringGetUTF8CString(msgArgumentJSRef, stringBuffer.get(), length);
	MessageBoxA(NULL, stringBuffer.get(), "NativeMessageBox", 0);
	return JSValueMakeNull(ctx_);
}

void HtmlView::RegisterNativeCFunctions()
{
	JSContextRef ctx = view->js_context(); // Create a JavaScript String containing the name of our callback.
	JSStringRef name = JSStringCreateWithUTF8CString("NativeMessageBox");
	JSObjectRef func = JSObjectMakeFunctionWithCallback(ctx, name, NativeMessageBox); // Create a garbage-collected JavaScript function
	JSObjectRef globalObj = JSContextGetGlobalObject(ctx); // Get the global JavaScript object (aka 'window')
	JSObjectSetProperty(ctx, globalObj, name, func, 0, 0); // Store our function in the page's global JavaScript object so that it accessible from the page as 'OnButtonClick()'.
	JSStringRelease(name); // Release the JavaScript String we created earlier.
}
