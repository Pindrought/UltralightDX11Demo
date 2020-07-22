#include "GPUDriverImpl.h"

void GPUDriverImpl::BeginSynchronize()
{
}

void GPUDriverImpl::EndSynchronize()
{
}

uint32_t GPUDriverImpl::NextTextureId()
{
	return nextTextureId++;
}

uint32_t GPUDriverImpl::NextRenderBufferId()
{
	return nextGeometryId++;
}

uint32_t GPUDriverImpl::NextGeometryId()
{
	return nextGeometryId++;
}

void GPUDriverImpl::UpdateCommandList(const ul::CommandList& list_)
{
	if (list_.size)
	{
		commandList.resize(list_.size);
		memcpy(&commandList[0], list_.commands, sizeof(ul::Command) * list_.size);
	}
}

void GPUDriverImpl::DrawCommandList()
{
	if (commandList.empty())
		return;
	for (auto& cmd : commandList)
	{
		if (cmd.command_type == ul::kCommandType_DrawGeometry)
			DrawGeometry(cmd.geometry_id, cmd.indices_count, cmd.indices_offset, cmd.gpu_state);
		else if (cmd.command_type == ul::kCommandType_ClearRenderBuffer)
			ClearRenderBuffer(cmd.gpu_state.render_buffer_id);
	}
	commandList.clear();
}

void GPUDriverImpl::DrawGeometry(uint32_t geometryId, uint32_t indexCount, uint32_t indexOffset, const ul::GPUState& state)
{
	//Implementation is overriden in GPUDriverD3D11
}

void GPUDriverImpl::ClearRenderBuffer(uint32_t renderBufferId)
{
	//Implementation is overriden in GPUDriverD3D11
}
