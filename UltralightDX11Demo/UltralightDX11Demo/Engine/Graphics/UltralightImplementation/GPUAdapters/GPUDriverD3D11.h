#pragma once
#include "GPUDriverImpl.h"
#include "../../D3D/D3DClass.h"
#include "../../D3D/Shaders/VertexShader.h"
#include "../../D3D/Shaders/PixelShader.h"
#include "../../D3D/ConstantBuffer.h"
#include "MapEntry.h"

class GPUDriverD3D11 : public GPUDriverImpl
{
public:
    GPUDriverD3D11(D3DClass* pd3d);
    //Inherited from GPUDriver
    virtual void CreateTexture(uint32_t textureId, ul::Ref<ul::Bitmap> bitmap) override;
    virtual void UpdateTexture(uint32_t textureId, ul::Ref<ul::Bitmap> bitmap) override;
    virtual void DestroyTexture(uint32_t textureId) override;
    virtual void CreateRenderBuffer(uint32_t renderBufferId, const ul::RenderBuffer& buffer) override;
    virtual void DestroyRenderBuffer(uint32_t renderBufferId) override;
    virtual void CreateGeometry(uint32_t geometryId, const ul::VertexBuffer& vertices, const ul::IndexBuffer& indices) override;
    virtual void UpdateGeometry(uint32_t geometryId, const ul::VertexBuffer& vertices, const ul::IndexBuffer& indices) override;
    virtual void DestroyGeometry(uint32_t geometryId) override;
    //Inherited from GPUDriverImpl
    void DrawGeometry(uint32_t geometryId, uint32_t indexCount, uint32_t indexOffset, const ul::GPUState& state) override;
    void ClearRenderBuffer(uint32_t renderBufferId) override;
    //
    ID3D11ShaderResourceView** GetAddressOfShaderResourceView(ul::RefPtr<ul::View>& view);
private:
    void LoadShaders();
    void InitializeSamplerState();
    void InitializeBlendStates();
    void InitializeRasterizerStates();

    void BindRenderBuffer(uint32_t renderBufferId);
    void SetViewport(uint32_t width, uint32_t height);
    void BindTexture(uint8_t textureUnit, uint32_t textureId);
    void UpdateConstantBuffer(const ul::GPUState& state);
    void BindGeometry(uint32_t geometryId);
    ul::Matrix ApplyProjection(const ul::Matrix4x4& transform, float screenWidth, float screenHeight);
    std::map<uint32_t, GeometryEntry> geometryMap;
    std::map<uint32_t, RenderTargetEntry> renderTargetMap;
    std::map<uint32_t, TextureEntry> textureMap;
    VertexShader vs_fill;
    VertexShader vs_fillPath;
    PixelShader ps_fill;
    PixelShader ps_fillPath;
    ConstantBuffer<ConstantBufferType_Ultralight> constantBuffer;
    ComPtr<ID3D11SamplerState> samplerState;
    ComPtr<ID3D11BlendState> blendState_disableBlend;
    ComPtr<ID3D11BlendState> blendState_enableBlend;
    ComPtr<ID3D11RasterizerState> rasterizerState_default;
    ComPtr<ID3D11RasterizerState> rasterizerState_scissor;

    D3DClass* pd3d = nullptr;
};