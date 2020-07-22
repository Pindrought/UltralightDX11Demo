#include "GPUDriverD3D11.h"
#include "../../D3D/InputLayoutDescriptions.h"

GPUDriverD3D11::GPUDriverD3D11(D3DClass* pd3d_)
{
    pd3d = pd3d_;
    LoadShaders();
    InitializeSamplerState();
    InitializeBlendStates();
    InitializeRasterizerStates();

    HRESULT hr = constantBuffer.Initialize(pd3d->device.Get(), pd3d->deviceContext.Get());
    FatalErrorIfFail(hr, "GPUDriverD3D11::GPUDriverD3D11 failed to initialize constant buffer.");
}

void GPUDriverD3D11::CreateTexture(uint32_t textureId_, ul::Ref<ul::Bitmap> bitmap_)
{
    auto i = textureMap.find(textureId_);
    if (i != textureMap.end())
        FatalError("GPUDriverD3D11::CreateTexture, texture id already exists.");

    if (!(bitmap_->format() == ul::kBitmapFormat_BGRA8_UNORM_SRGB || bitmap_->format() == ul::kBitmapFormat_A8_UNORM))
        FatalError("GPUDriverD3D11::CreateTexture, unsupported format.");

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = bitmap_->width();
    desc.Height = bitmap_->height();
    desc.MipLevels = desc.ArraySize = 1;
    desc.Format = bitmap_->format() == ul::kBitmapFormat_BGRA8_UNORM_SRGB ? DXGI_FORMAT_B8G8R8A8_UNORM_SRGB : DXGI_FORMAT_A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;

    auto& textureEntry = textureMap[textureId_];
    HRESULT hr;

    if (bitmap_->IsEmpty())
    {
        desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.CPUAccessFlags = 0;

#if ENABLE_MSAA
        desc.SampleDesc.Count = 8;
        desc.SampleDesc.Quality = D3D11_STANDARD_MULTISAMPLE_PATTERN;

        textureEntry.isMSAARenderTarget = true;
#endif

        hr = pd3d->device->CreateTexture2D(&desc, NULL, &textureEntry.texture);
    }
    else
    {
        D3D11_SUBRESOURCE_DATA textureData;
        ZeroMemory(&textureData, sizeof(textureData));
        textureData.pSysMem = bitmap_->LockPixels();
        textureData.SysMemPitch = bitmap_->row_bytes();
        textureData.SysMemSlicePitch = (UINT)bitmap_->size();

        hr = pd3d->device->CreateTexture2D(&desc, &textureData, &textureEntry.texture);
        bitmap_->UnlockPixels();
    }
    FatalErrorIfFail(hr, "GPUDriverD3D11::CreateTexture, unable to create texture.");


    D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
    ZeroMemory(&srv_desc, sizeof(srv_desc));
    srv_desc.Format = desc.Format;
    srv_desc.ViewDimension = textureEntry.isMSAARenderTarget ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
    srv_desc.Texture2D.MostDetailedMip = 0;
    srv_desc.Texture2D.MipLevels = 1;

    hr = pd3d->device->CreateShaderResourceView(textureEntry.texture.Get(), &srv_desc, &textureEntry.textureSRV);
    FatalErrorIfFail(hr, "GPUDriverD3D11::CreateTexture, unable to create shader resource view for texture.");

#if ENABLE_MSAA
    if (textureEntry.isMSAARenderTarget)
    {
        // Create resolve texture and shader resource view

        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        hr = pd3d->device->CreateTexture2D(&desc, NULL, &textureEntry.resolveTexture);
        FatalErrorIfFail(hr, "GPUDriverD3D11::CreateTexture, unable to create MSAA resolve texture.");

        srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

        hr = pd3d->device->CreateShaderResourceView(textureEntry.resolveTexture.Get(), &srv_desc, &textureEntry.resolveSRV);
        FatalErrorIfFail(hr, "GPUDriverD3D11::CreateTexture, unable to create shader resource view for MSAA resolve texture.");
    }
#endif
}

void GPUDriverD3D11::UpdateTexture(uint32_t textureId_, ul::Ref<ul::Bitmap> bitmap_)
{
    auto iter = textureMap.find(textureId_);
    if (iter == textureMap.end())
        FatalError("GPUDriverD3D11::UpdateTexture, texture id doesn't exist.");

    auto& entry = iter->second;
    D3D11_MAPPED_SUBRESOURCE res;
    pd3d->deviceContext->Map(entry.texture.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &res);

    if (res.RowPitch == bitmap_->row_bytes())
    {
        memcpy(res.pData, bitmap_->LockPixels(), bitmap_->size());
        bitmap_->UnlockPixels();
    }
    else
    {
        ul::Ref<ul::Bitmap> mapped_bitmap = ul::Bitmap::Create(bitmap_->width(), bitmap_->height(), bitmap_->format(),
            res.RowPitch, res.pData, res.RowPitch * bitmap_->height(), false);
        ul::IntRect dest_rect = { 0, 0, (int)bitmap_->width(), (int)bitmap_->height() };
        mapped_bitmap->DrawBitmap(dest_rect, dest_rect, bitmap_, false);
    }

    pd3d->deviceContext->Unmap(entry.texture.Get(), 0);
}

void GPUDriverD3D11::DestroyTexture(uint32_t textureId_)
{
    auto iter = textureMap.find(textureId_);
    if (iter != textureMap.end())
    {
        textureMap.erase(iter);
    }
}

void GPUDriverD3D11::CreateRenderBuffer(uint32_t renderBufferId_, const ul::RenderBuffer& buffer_)
{
    if (renderBufferId_ == 0)
        FatalError("GPUDriverD3D11::CreateRenderBuffer, render buffer ID 0 is reserved for default render target view.");

    auto i = renderTargetMap.find(renderBufferId_);
    if (i != renderTargetMap.end())
        FatalError("GPUDriverD3D11::CreateRenderBuffer, render buffer id already exists.");

    auto textureEntry = textureMap.find(buffer_.texture_id);
    if (textureEntry == textureMap.end())
        FatalError("GPUDriverD3D11::CreateRenderBuffer, texture id doesn't exist.");

    D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc = {};
    renderTargetViewDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
#if ENABLE_MSAA
    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
#endif

    ComPtr<ID3D11Texture2D> tex = textureEntry->second.texture;
    auto& render_target_entry = renderTargetMap[renderBufferId_];
    HRESULT hr = pd3d->device->CreateRenderTargetView(tex.Get(), &renderTargetViewDesc, render_target_entry.renderTargetView.GetAddressOf());

    render_target_entry.renderTargetTextureId = buffer_.texture_id;
    FatalErrorIfFail(hr, "GPUDriverD3D11::CreateRenderBuffer, unable to create render target.");
}

void GPUDriverD3D11::DestroyRenderBuffer(uint32_t renderBufferId_)
{
    auto iter = renderTargetMap.find(renderBufferId_);
    if (iter != renderTargetMap.end())
    {
        iter->second.renderTargetView.Reset();
        renderTargetMap.erase(iter);
    }
}

void GPUDriverD3D11::CreateGeometry(uint32_t geometryId_, const ul::VertexBuffer& vertices_, const ul::IndexBuffer& indices_)
{
    if (geometryMap.find(geometryId_) != geometryMap.end())
        FatalError("GPUDriverD3D11::CreateGeometry called with a geometry id that already exists.");

    GeometryEntry geometry;
    geometry.format = vertices_.format;

    HRESULT hr;

    D3D11_BUFFER_DESC vertex_desc = {};
    vertex_desc.Usage = D3D11_USAGE_DYNAMIC;
    vertex_desc.ByteWidth = vertices_.size;
    vertex_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertex_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    D3D11_SUBRESOURCE_DATA vertex_data = {};
    vertex_data.pSysMem = vertices_.data;

    hr = pd3d->device->CreateBuffer(&vertex_desc, &vertex_data, geometry.vertexBuffer.GetAddressOf());
    FatalErrorIfFail(hr, "GPUDriverD3D11::CreateGeometry CreateBuffer for vertex buffer failed.");

    D3D11_BUFFER_DESC index_desc = {};
    index_desc.Usage = D3D11_USAGE_DYNAMIC;
    index_desc.ByteWidth = indices_.size;
    index_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    index_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    D3D11_SUBRESOURCE_DATA index_data = {};
    index_data.pSysMem = indices_.data;

    hr = pd3d->device->CreateBuffer(&index_desc, &index_data, geometry.indexBuffer.GetAddressOf());
    FatalErrorIfFail(hr, "GPUDriverD3D11::CreateGeometry CreateBuffer for index buffer failed.");

    geometryMap.insert({ geometryId_, std::move(geometry) });
}

void GPUDriverD3D11::UpdateGeometry(uint32_t geometryId_, const ul::VertexBuffer& vertices_, const ul::IndexBuffer& indices_)
{
    auto iter = geometryMap.find(geometryId_);
    if (iter == geometryMap.end())
        FatalError("GPUDriverD3D11::UpdateGeometry, geometry id doesn't exist.");

    auto& entry = iter->second;
    D3D11_MAPPED_SUBRESOURCE res;

    pd3d->deviceContext->Map(entry.vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
    memcpy(res.pData, vertices_.data, vertices_.size);
    pd3d->deviceContext->Unmap(entry.vertexBuffer.Get(), 0);

    pd3d->deviceContext->Map(entry.indexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
    memcpy(res.pData, indices_.data, indices_.size);
    pd3d->deviceContext->Unmap(entry.indexBuffer.Get(), 0);
}

void GPUDriverD3D11::DestroyGeometry(uint32_t geometryId_)
{
    auto iter = geometryMap.find(geometryId_);
    if (iter != geometryMap.end())
    {
        iter->second.vertexBuffer.Reset();
        iter->second.indexBuffer.Reset();
        geometryMap.erase(iter);
    }
}

void GPUDriverD3D11::DrawGeometry(uint32_t geometryId_, uint32_t indexCount_, uint32_t indexOffset_, const ul::GPUState& state_)
{
    BindRenderBuffer(state_.render_buffer_id);

    SetViewport(state_.viewport_width, state_.viewport_height);

    if (state_.texture_1_id)
        BindTexture(0, state_.texture_1_id);

    if (state_.texture_2_id)
        BindTexture(1, state_.texture_2_id);

    UpdateConstantBuffer(state_);

    BindGeometry(geometryId_);

    pd3d->deviceContext->PSSetSamplers(0, 1, samplerState.GetAddressOf());

    //Bind Vertex/Pixel Shaders
    if (state_.shader_type == ul::ShaderType::kShaderType_Fill)
    {
        pd3d->deviceContext->VSSetShader(vs_fill.GetShader(), nullptr, 0);
        pd3d->deviceContext->PSSetShader(ps_fill.GetShader(), nullptr, 0);
    }
    else //fillpath
    {
        pd3d->deviceContext->VSSetShader(vs_fillPath.GetShader(), nullptr, 0);
        pd3d->deviceContext->PSSetShader(ps_fillPath.GetShader(), nullptr, 0);
    }

    if (state_.enable_blend)
        pd3d->deviceContext->OMSetBlendState(blendState_enableBlend.Get(), NULL, 0xFFFFFFFF);
    else
        pd3d->deviceContext->OMSetBlendState(blendState_disableBlend.Get(), NULL, 0xFFFFFFFF);

    if (state_.enable_scissor)
    {
        pd3d->deviceContext->RSSetState(rasterizerState_scissor.Get());
        D3D11_RECT scissor_rect =
        {
          (LONG)(state_.scissor_rect.left),
          (LONG)(state_.scissor_rect.top),
          (LONG)(state_.scissor_rect.right),
          (LONG)(state_.scissor_rect.bottom)
        };

        pd3d->deviceContext->RSSetScissorRects(1, &scissor_rect);
    }
    else
    {
        pd3d->deviceContext->RSSetState(rasterizerState_default.Get());
    }

    pd3d->deviceContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
    pd3d->deviceContext->PSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
    pd3d->deviceContext->DrawIndexed(indexCount_, indexOffset_, 0);
}

void GPUDriverD3D11::ClearRenderBuffer(uint32_t renderBufferId_)
{
    float color[4] = { 0.0f, 0.0f, 0.0f, 0.0f }; //Clear to black

    if (renderBufferId_ == 0)
    {
        pd3d->deviceContext->ClearRenderTargetView(pd3d->renderTargetView.Get(), color);
        return;
    }

    auto renderTargetIter = renderTargetMap.find(renderBufferId_);
    if (renderTargetIter == renderTargetMap.end())
    {
        FatalError("GPUDriverD3D11::ClearRenderBuffer, render buffer id doesn't exist.");
    }

    pd3d->deviceContext->ClearRenderTargetView(renderTargetIter->second.renderTargetView.Get(), color);

#if ENABLE_MSAA
    auto textureIter = textureMap.find(renderTargetIter->second.renderTargetTextureId);
    if (textureIter == textureMap.end())
        FatalError("GPUDriverD3D11::ClearRenderBuffer, render target texture id doesn't exist.");

    // Flag the MSAA render target texture for Resolve when we bind it to a shader for reading later.
    textureIter->second.needsResolve = true;
#endif
}

ID3D11ShaderResourceView** GPUDriverD3D11::GetAddressOfShaderResourceView(ul::RefPtr<ul::View>& view_)
{
    auto textureId = view_->render_target().texture_id;
    auto iter = textureMap.find(textureId);
    if (iter == textureMap.end())
        return nullptr;
    return iter->second.textureSRV.GetAddressOf();
}

void GPUDriverD3D11::LoadShaders()
{
    HRESULT hr;
    //Fill Path Vertex Shader
    std::wstring fillPathVertexShaderPath = L"vs_ul_v2f_c4f_t2f.cso";
    hr = vs_fillPath.Initialize(pd3d->device.Get(), fillPathVertexShaderPath, InputLayoutDescription_ultralight_2f_4ub_2f);
    FatalErrorIfFail(hr, "Failed to initialize fill path vertex shader.");
    //Fill Path Pixel Shader
    std::wstring fillPathPixelShaderPath = L"ps_ul_fill_path.cso";
    hr = ps_fillPath.Initialize(pd3d->device.Get(), fillPathPixelShaderPath);
    FatalErrorIfFail(hr, "Failed to initialize fill path pixel shader.");

    //Fill Vertex Shader
    std::wstring fillVertexShaderPath = L"vs_ul_v2f_c4f_t2f_t2f_d28f.cso";
    hr = vs_fill.Initialize(pd3d->device.Get(), fillVertexShaderPath, InputLayoutDescription_ultralight_2f_4ub_2f_2f_28f);
    FatalErrorIfFail(hr, "Failed to initialize fill path vertex shader.");
    //Fill Pixel Shader
    std::wstring fillPixelShaderPath = L"ps_ul_fill.cso";
    hr = ps_fill.Initialize(pd3d->device.Get(), fillPixelShaderPath);
    FatalErrorIfFail(hr, "Failed to initialize fill path pixel shader.");
}

void GPUDriverD3D11::InitializeSamplerState()
{
    D3D11_SAMPLER_DESC sampler_desc = {};
    sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampler_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampler_desc.MinLOD = 0;
    HRESULT hr = pd3d->device->CreateSamplerState(&sampler_desc, &samplerState);
    FatalErrorIfFail(hr, "GPUDriverD3D11::InitializeSamplerAndBlendStates Failed to create sampler state.");
}

void GPUDriverD3D11::InitializeBlendStates()
{
    //Create enabled blend state
    CD3D11_BLEND_DESC blend_desc_enabled(D3D11_DEFAULT);
    blend_desc_enabled.RenderTarget[0].BlendEnable = TRUE; //Disabled by default
    blend_desc_enabled.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blend_desc_enabled.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_INV_DEST_ALPHA;
    blend_desc_enabled.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;

    HRESULT hr = pd3d->device->CreateBlendState(&blend_desc_enabled, &blendState_enableBlend);
    FatalErrorIfFail(hr, "GPUDriverD3D11::InitializeBlendStates failed to create enabled blend state");

    //Create disabled blend state
    CD3D11_BLEND_DESC blend_desc_disabled(D3D11_DEFAULT); //disabled blend by default

    hr = pd3d->device->CreateBlendState(&blend_desc_disabled, &blendState_disableBlend);
    FatalErrorIfFail(hr, "GPUDriverD3D11::InitializeBlendStates failed to create disabled blend state");
}

void GPUDriverD3D11::InitializeRasterizerStates()
{
    HRESULT hr;
    //Create default rasterizer state (no scissor)
    CD3D11_RASTERIZER_DESC rasterizerDesc_default(D3D11_DEFAULT);
    rasterizerDesc_default.CullMode = D3D11_CULL_NONE;
    rasterizerDesc_default.DepthClipEnable = FALSE;
#if ENABLE_MSAA
    rasterizerDesc_default.MultisampleEnable = true;
    rasterizerDesc_default.AntialiasedLineEnable = true;
#endif

    hr = pd3d->device->CreateRasterizerState(&rasterizerDesc_default, &rasterizerState_default);
    FatalErrorIfFail(hr, "GPUDriverD3D11::InitializeRasterizerStates failed to create default rasterizer state.");

    CD3D11_RASTERIZER_DESC rasterizerDesc_scissor(D3D11_DEFAULT);
    rasterizerDesc_scissor.CullMode = D3D11_CULL_NONE;
    rasterizerDesc_scissor.DepthClipEnable = FALSE;
    rasterizerDesc_scissor.ScissorEnable = true;
#if ENABLE_MSAA
    rasterizerDesc_scissor.MultisampleEnable = true;
    rasterizerDesc_scissor.AntialiasedLineEnable = true;
#endif

    hr = pd3d->device->CreateRasterizerState(&rasterizerDesc_scissor, &rasterizerState_scissor);
    FatalErrorIfFail(hr, "GPUDriverD3D11::InitializeRasterizerStates failed to create scissored rasterizer state.");
}

void GPUDriverD3D11::BindRenderBuffer(uint32_t renderBufferId_)
{
    // Unbind any textures/shader resources to avoid warnings in case a render buffer that we would like to bind is already bound as an input texture.
    ID3D11ShaderResourceView* nullSRV[3] = { nullptr, nullptr, nullptr };
    pd3d->deviceContext->PSSetShaderResources(0, 3, nullSRV);

    ID3D11RenderTargetView* target;
    if (renderBufferId_ == 0)
    {
        target = pd3d->renderTargetView.Get();
    }
    else
    {
        auto renderTarget = renderTargetMap.find(renderBufferId_);
        if (renderTarget == renderTargetMap.end())
            FatalError("GPUDriverD3D11::BindRenderBuffer, render buffer id doesn't exist.");

        target = renderTarget->second.renderTargetView.Get();

#if ENABLE_MSAA
        auto renderTargetTexture = textureMap.find(renderTarget->second.renderTargetTextureId);
        if (renderTargetTexture == textureMap.end())
            FatalError("GPUDriverD3D11::BindRenderBuffer, render target texture id doesn't exist.");

        // Flag the MSAA render target texture for Resolve when we bind it to a shader for reading later.
        renderTargetTexture->second.needsResolve = true;
#endif
    }

    pd3d->deviceContext->OMSetRenderTargets(1, &target, nullptr);
}

void GPUDriverD3D11::SetViewport(uint32_t width_, uint32_t height_)
{
    D3D11_VIEWPORT vp = {};
    vp.Width = (float)width_;
    vp.Height = (float)height_;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    pd3d->deviceContext->RSSetViewports(1, &vp);
}

void GPUDriverD3D11::BindTexture(uint8_t textureUnit_, uint32_t textureId_)
{
    auto iter = textureMap.find(textureId_);
    if (iter == textureMap.end())
        FatalError("GPUDriverD3D11::BindTexture, texture id doesn't exist.");

    auto& entry = iter->second;

    if (entry.isMSAARenderTarget)
    {
        if (entry.needsResolve)
        {
            pd3d->deviceContext->ResolveSubresource(entry.resolveTexture.Get(), 0, entry.texture.Get(), 0, DXGI_FORMAT_B8G8R8A8_UNORM_SRGB);
            entry.needsResolve = false;
        }
        pd3d->deviceContext->PSSetShaderResources(textureUnit_, 1, entry.resolveSRV.GetAddressOf());
    }
    else
    {
        pd3d->deviceContext->PSSetShaderResources(textureUnit_, 1, entry.textureSRV.GetAddressOf());
    }
}

void GPUDriverD3D11::UpdateConstantBuffer(const ul::GPUState& state_)
{
    float screenWidth = (float)state_.viewport_width;
    float screenHeight = (float)state_.viewport_height;
    ul::Matrix modelViewProjectionMat = ApplyProjection(state_.transform, screenWidth, screenHeight);

    auto& cbdata = constantBuffer.data;
    cbdata.State = { 0.0f, screenWidth, screenHeight, 1.0f };

    cbdata.Transform = DirectX::XMMATRIX(modelViewProjectionMat.GetMatrix4x4().data);
    cbdata.Scalar4[0] =
    {
        state_.uniform_scalar[0],
        state_.uniform_scalar[1],
        state_.uniform_scalar[2],
        state_.uniform_scalar[3]
    };
    cbdata.Scalar4[1] =
    {
        state_.uniform_scalar[4],
        state_.uniform_scalar[5],
        state_.uniform_scalar[6],
        state_.uniform_scalar[7]
    };
    for (size_t i = 0; i < 8; ++i)
        cbdata.Vector[i] = DirectX::XMFLOAT4(state_.uniform_vector[i].value);
    cbdata.ClipSize = state_.clip_size;
    for (size_t i = 0; i < state_.clip_size; ++i)
        cbdata.Clip[i] = DirectX::XMMATRIX(state_.clip[i].data);
    constantBuffer.ApplyChanges();
}

void GPUDriverD3D11::BindGeometry(uint32_t geometryId_)
{
    auto iter = geometryMap.find(geometryId_);
    if (iter == geometryMap.end())
        FatalError("GPUDriverD3D11::BindGeometry geometry id does not exist in geometry map.");

    auto& geometry = iter->second;
    UINT stride = geometry.format == ul::kVertexBufferFormat_2f_4ub_2f ? sizeof(ul::Vertex_2f_4ub_2f) : sizeof(ul::Vertex_2f_4ub_2f_2f_28f);
    UINT offset = 0;
    pd3d->deviceContext->IASetVertexBuffers(0, 1, geometry.vertexBuffer.GetAddressOf(), &stride, &offset);
    pd3d->deviceContext->IASetIndexBuffer(geometry.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    pd3d->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    if (geometry.format == ul::kVertexBufferFormat_2f_4ub_2f)
        pd3d->deviceContext->IASetInputLayout(vs_fillPath.GetInputLayout());
    else
        pd3d->deviceContext->IASetInputLayout(vs_fill.GetInputLayout());
}

ul::Matrix GPUDriverD3D11::ApplyProjection(const ul::Matrix4x4& transform_, float screenWidth_, float screenHeight_)
{
    ul::Matrix transformMatrix;
    transformMatrix.Set(transform_);

    ul::Matrix result;
    result.SetOrthographicProjection(screenWidth_, screenHeight_, false);
    result.Transform(transformMatrix);

    return result;
}
