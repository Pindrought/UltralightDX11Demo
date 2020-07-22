#pragma once
#include "../../../../pch.h"

class GPUDriverImpl : public ul::GPUDriver
{
public:
    //Inherited from GPUDriver
    virtual void BeginSynchronize() override;
    virtual void EndSynchronize() override;
    virtual uint32_t NextTextureId() override;
    virtual uint32_t NextRenderBufferId() override;
    virtual uint32_t NextGeometryId() override;
    virtual void UpdateCommandList(const ul::CommandList& list) override;
    //
    void DrawCommandList();
    virtual void DrawGeometry(uint32_t geometryId, uint32_t indexCount, uint32_t indexOffset, const ul::GPUState& state);
    virtual void ClearRenderBuffer(uint32_t renderBufferId);
private:
    uint32_t nextTextureId = 1;
    uint32_t nextRenderBufferId = 1;
    uint32_t nextGeometryId = 1;
    std::vector<ul::Command> commandList;
};