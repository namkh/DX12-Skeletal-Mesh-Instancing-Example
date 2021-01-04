#include "RenderItem.h"

bool RenderItem::Initialize(RenderObject* renderObj,
							StructuredBuffer<GlobalConstants>* globalConstantBuffer,
							uint32_t frameCount)
{
	if (renderObj == nullptr)
	{
		REPORT(EReportType::REPORT_TYPE_WARN, "Render Object is null.");
		return false;
	}
	m_renderObj = renderObj;
	m_globalConstntBuffer = globalConstantBuffer;
	m_renderCommandBuffers.Initialize(frameCount);

	if (!CreateShader())
	{
		return false;
	}

	if (!CreateRootSignature())
	{
		return false;
	}

	if (!CreateDescriptors())
	{
		return false;
	}

	if (!CreatePSO())
	{
		return false;
	}

	return true;
}

void RenderItem::UpdateResources(CommandBuffer* cmdBuf)
{
}

void RenderItem::Destroy()
{
	m_renderCommandBuffers.Destory();

	if (m_shader != nullptr)
	{
		delete m_shader;
	}
	if (m_rootSignature != nullptr)
	{
		m_rootSignature->Release();
	}
	if (m_pso)
	{
		m_pso->Release();
	}
}

void RenderItem::RefreshRenderCommandBuffer()
{
	CreateCommandBuffer();
}

bool RenderItem_SkeletalMeshInstancing::Initialize(RenderObject* renderObj,
												   StructuredBuffer<GlobalConstants>* globalConstantBuffer,
												   uint32_t frameCount)
{
	if (renderObj != nullptr)
	{
		m_renderObj = renderObj;
		renderObj->CollectMaterials(m_materials);
		for (auto& cur : m_materials)
		{
			if (cur != nullptr && cur->m_diffuseTex != nullptr)
			{
				auto iterFind = m_textures.find(cur->m_diffuseTex->GetUID());
				if (iterFind == m_textures.end())
				{
					m_textures.insert(std::make_pair(cur->m_diffuseTex->GetUID(), cur->m_diffuseTex));
				}
			}
		}

		if (!CreateBuffers())
		{
			return false;
		}

		if (RenderItem::Initialize(renderObj, globalConstantBuffer, frameCount))
		{
			SkeletalMeshRenderObject* skelMeshRenderObj = dynamic_cast<SkeletalMeshRenderObject*>(m_renderObj);
			if (skelMeshRenderObj != nullptr)
			{
				SingleTimeCommandBuffer cmdBuf;
				cmdBuf.Begin(nullptr);
				if (!m_animUpdator.Initialize(skelMeshRenderObj, &cmdBuf, frameCount))
				{
					return false;
				}
				//UpdateInstanceResources(&cmdBuf);
				cmdBuf.End();
			}
			CreateCommandBuffer();

			return true;
		}
		return false;
	}

	return false;
}

void RenderItem_SkeletalMeshInstancing::Destroy()
{
	RenderItem::Destroy();

	m_textureDescriptorHeap->Release();

	m_instanceDatasBuffer.Destroy();
	m_materialDatasBuffer.Destroy();
	
	m_animUpdator.Destroy();
}

void RenderItem_SkeletalMeshInstancing::UpdateInstanceResources(CommandBufferBase* cmdBuf)
{
	if (m_renderObj != nullptr)
	{
		m_animUpdator.UpdateResources(cmdBuf);

		uint32_t skeletonOffset = 0;
		uint32_t numInstances = m_renderObj->GetInstanceCount();
		if (numInstances > 0 && numInstances == m_instanceDatas.size())
		{
			for (uint32_t i = 0; i < numInstances; i++)
			{
				SkeletalMeshRenderObjectInstance* instance = dynamic_cast<SkeletalMeshRenderObjectInstance*>(m_renderObj->GetInstance(i));
				int rootMaterialIndex = GetMaterialBindIndex(m_renderObj->GetMaterial());
				if (instance != nullptr)
				{
					m_instanceDatas[i].WorldMat = XMMatrixTranspose(instance->GetWorldMatrix());
					if (instance->HasOverrideMaterial())
					{
						m_instanceDatas[i].MaterialID = GetMaterialBindIndex(instance->GetOverrideMaterial());
					}
					else
					{
						m_instanceDatas[i].MaterialID = rootMaterialIndex;
					}
					m_instanceDatas[i].SkeletonOffset = skeletonOffset;
					SimpleSkeleton* skeleton = instance->GetSkeleton();
					if (skeleton != nullptr)
					{
						skeletonOffset += skeleton->GetBoneCount();
					}
				}
			}
			m_instanceDatasBuffer.UploadResource(cmdBuf, reinterpret_cast<void*>(m_instanceDatas.data()));
		}
		if (m_materials.size() > 0 && m_materialDatas.size() > 0 && m_materials.size() == m_materialDatas.size())
		{
			uint32_t numMaterials = static_cast<uint32_t>(m_materialDatas.size());
			for (uint32_t i = 0; i < numMaterials; i++)
			{
				m_materialDatas[i].DiffuseTexIndex = GetTextureBindIndex(m_materials[i]->m_diffuseTex);
			}
			m_materialDatasBuffer.UploadResource(cmdBuf, reinterpret_cast<void*>(m_materialDatas.data()));
		}
	}
}

void RenderItem_SkeletalMeshInstancing::UpdateResources(CommandBuffer* cmdBuf)
{
	RenderItem::UpdateResources(cmdBuf);
	
	if (m_renderObj != nullptr)
	{
		UpdateInstanceResources(cmdBuf);	
	}
}

bool RenderItem_SkeletalMeshInstancing::CreateBuffers()
{
	if (m_renderObj != nullptr)
	{
		uint32_t numInstances = m_renderObj->GetInstanceCount();
		if (numInstances > 0)
		{
			m_instanceDatas.resize(numInstances);
			m_instanceDatasBuffer.Initialize(numInstances, false);
		}

		m_materialDatas.resize(m_materials.size());
		m_materialDatasBuffer.Initialize(static_cast<uint32_t>(m_materials.size()), false);
		return true;
	}
	return false;
}


bool RenderItem_SkeletalMeshInstancing::CreateDescriptors()
{
	if (m_renderObj == nullptr)
	{
		REPORT(EReportType::REPORT_TYPE_ERROR, "Render object is null.");
		return false;
	}

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
	heapDesc.NumDescriptors = static_cast<uint32_t>(m_textures.size());
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	heapDesc.NodeMask = 0;
	gLogicalDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_textureDescriptorHeap));

	CD3DX12_CPU_DESCRIPTOR_HANDLE handle(m_textureDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	for (auto& cur : m_textures)
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = cur.second->GetTextureResource()->GetDesc().Format;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = cur.second->GetTextureResource()->GetDesc().MipLevels;
		srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		gLogicalDevice->CreateShaderResourceView(cur.second->GetTextureResource(), &srvDesc, handle);

		handle.Offset(1, gDX12DeviceRes->GetCbvSrvUavDiscriptorSize());
	}
	return true;
}

CommandBufferBase* RenderItem_SkeletalMeshInstancing::GetAnimUpdateCommandBuffer(int frameIndex)
{
	return m_animUpdator.GetAnimUpdateCommandBuffer(frameIndex);
}

int RenderItem_SkeletalMeshInstancing::GetMaterialBindIndex(SimpleMaterial* material)
{
	for (int i = 0; i < m_materials.size(); i++)
	{
		if (m_materials[i]->GetUID() == material->GetUID())
		{
			return i;
		}
	}
	return INVALID_INDEX_INT;
}

int RenderItem_SkeletalMeshInstancing::GetTextureBindIndex(SimpleTexture2D* texture)
{
	if (texture != nullptr)
	{
		uint32_t bindIndex = 0;
		for (auto& cur : m_textures)
		{
			if (cur.second->GetUID() == texture->GetUID())
			{
				return bindIndex;
			}
			bindIndex++;
		}
	}
	return INVALID_INDEX_INT;
}

const wchar_t* RenderItem_HardwareInstancing::HARDWARE_INSTANCING_VS_FILE_PATH = L"../Resources/Shaders/HardwareInstancing/HardwareInstancing_VS.cso";
const wchar_t* RenderItem_HardwareInstancing::HARDWARE_INSTANCING_PS_FILE_PATH = L"../Resources/Shaders/HardwareInstancing/HardwareInstancing_PS.cso";

bool RenderItem_HardwareInstancing::CreateShader()
{
	m_shader = new SimpleShader(static_cast<uint8_t>(EShaderStageType::VERTEX_SHADER) | static_cast<uint8_t>(EShaderStageType::PIXEL_SHADER));
	if (m_shader != nullptr)
	{
		m_shader->LoadCompiledShader(EShaderStageType::VERTEX_SHADER, HARDWARE_INSTANCING_VS_FILE_PATH);
		m_shader->LoadCompiledShader(EShaderStageType::PIXEL_SHADER, HARDWARE_INSTANCING_PS_FILE_PATH);
		return true;
	}
	return false;
}

bool RenderItem_HardwareInstancing::CreateRootSignature()
{
	std::vector<CD3DX12_DESCRIPTOR_RANGE> textureRanges(m_textures.size());
	for (uint32_t i = 0; i < textureRanges.size(); i++)
	{
		textureRanges[i].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, i, 0);
	}

	std::vector<CD3DX12_ROOT_PARAMETER> slotRootParameterList;
	CD3DX12_ROOT_PARAMETER globalConstantSlotRootParameter = {};
	globalConstantSlotRootParameter.InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL);
	slotRootParameterList.push_back(globalConstantSlotRootParameter);
	
	CD3DX12_ROOT_PARAMETER instanceDataBufferSlotRootParameter = {};
	instanceDataBufferSlotRootParameter.InitAsShaderResourceView(0, 1, D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL);
	slotRootParameterList.push_back(instanceDataBufferSlotRootParameter);
	
	CD3DX12_ROOT_PARAMETER materialDataBufferSlotRootParameter = {};
	materialDataBufferSlotRootParameter.InitAsShaderResourceView(1, 1, D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL);
	slotRootParameterList.push_back(materialDataBufferSlotRootParameter);

	CD3DX12_ROOT_PARAMETER skeletonBufferSlotRootParameter = {};
	skeletonBufferSlotRootParameter.InitAsShaderResourceView(2, 1, D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL);
	slotRootParameterList.push_back(skeletonBufferSlotRootParameter);

	for (int i = 0; i < textureRanges.size(); i++)
	{
		CD3DX12_ROOT_PARAMETER textureSlotRootParameter = {};
		textureSlotRootParameter.InitAsDescriptorTable(1, &textureRanges[i], D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_PIXEL);
		slotRootParameterList.push_back(textureSlotRootParameter);
	}
	
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc
	(
		static_cast<uint32_t>(slotRootParameterList.size()),
		slotRootParameterList.data(),
		1,
		&m_linearWrapSampler,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
	);

	ID3DBlob* serializedRootSig = nullptr;
	ID3DBlob* errorBlob = nullptr;

	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &serializedRootSig, &errorBlob);
	if (errorBlob != nullptr)
	{
		REPORT(EReportType::REPORT_TYPE_ERROR, "Root signature serialize failed");
		return false;
	}

	HRESULT res = gLogicalDevice->CreateRootSignature(0, serializedRootSig->GetBufferPointer(), serializedRootSig->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature));
	if (FAILED(res))
	{
		REPORT(EReportType::REPORT_TYPE_ERROR, "Root signature create failed.");
		return false;
	}
	return true;
}

bool RenderItem_HardwareInstancing::CreatePSO()
{
	std::vector<D3D12_INPUT_ELEMENT_DESC>* inputLayout = nullptr;
	
	SimpleGeometry* geom = m_renderObj->GetGeometry();
	if (geom != nullptr)
	{
		inputLayout = geom->GetDefaultInputLayout();
	}

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	memset(&psoDesc, 0, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	if (inputLayout != nullptr)
	{
		psoDesc.InputLayout = { inputLayout->data(), (uint32_t)inputLayout->size() };
	}
	psoDesc.pRootSignature = m_rootSignature;
	psoDesc.VS =
	{
		reinterpret_cast<BYTE*>(m_shader->GetShader(EShaderStageType::VERTEX_SHADER)->GetBufferPointer()),
		m_shader->GetShader(EShaderStageType::VERTEX_SHADER)->GetBufferSize()
	};
	psoDesc.PS =
	{
		reinterpret_cast<BYTE*>(m_shader->GetShader(EShaderStageType::PIXEL_SHADER)->GetBufferPointer()),
		m_shader->GetShader(EShaderStageType::PIXEL_SHADER)->GetBufferSize()
	};
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = gDX12DeviceRes->GetSwapChain().GetBackBufferFormat();
	psoDesc.SampleDesc.Count = gDX12DeviceRes->GetMsaaState() ? 4 : 1;
	psoDesc.SampleDesc.Quality = gDX12DeviceRes->GetMsaaState() ? (gDX12DeviceRes->GetMsaaQuality() - 1) : 0;
	psoDesc.DSVFormat = gDX12DeviceRes->GetSwapChain().GetDepthStencilBufferFormat();

	HRESULT res = gLogicalDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pso));
	if (FAILED(res))
	{
		REPORT(EReportType::REPORT_TYPE_ERROR, "PSO create failed.");
		return false;
	}
	return true;
}

bool RenderItem_HardwareInstancing::CreateCommandBuffer()
{
	if (m_renderObj != nullptr)
	{
		SimpleGeometry* geometry = m_renderObj->GetGeometry();
		if(geometry != nullptr)
		{
			for (uint32_t i = 0; i < m_renderCommandBuffers.GetCommandBufferCount(); i++)
			{
				CommandBuffer* cmdBuffer = m_renderCommandBuffers.GetCommandBuffer(i);
				if (cmdBuffer != nullptr && cmdBuffer->Begin(m_pso))
				{
					ID3D12GraphicsCommandList* cmdBuf = cmdBuffer->GetCommandBuffer();
					cmdBuf->SetName(L"HI render cmd");
					cmdBuf->RSSetViewports(1, &gDX12DeviceRes->GetViewport());
					cmdBuf->RSSetScissorRects(1, &gDX12DeviceRes->GetScissorRect());

					cmdBuf->ResourceBarrier
					(
						1,
						&CD3DX12_RESOURCE_BARRIER::Transition
						(
							gDX12DeviceRes->GetSwapChain().GetBackBuffer(i),
							D3D12_RESOURCE_STATE_PRESENT,
							D3D12_RESOURCE_STATE_RENDER_TARGET
						)
					);

					cmdBuf->OMSetRenderTargets(1, &gDX12DeviceRes->GetSwapChain().GetRtvHandle(i), true, &gDX12DeviceRes->GetSwapChain().GetDsvHandle());
					cmdBuf->ClearRenderTargetView(gDX12DeviceRes->GetSwapChain().GetRtvHandle(i), Colors::Black, 0, nullptr);
					cmdBuf->ClearDepthStencilView(gDX12DeviceRes->GetSwapChain().GetDsvHandle(),
													D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
													1.0f, 0, 0, nullptr);

					cmdBuf->SetGraphicsRootSignature(m_rootSignature);
					cmdBuf->SetGraphicsRootConstantBufferView(0, m_globalConstntBuffer->GetResource()->GetGPUVirtualAddress());
					cmdBuf->SetGraphicsRootShaderResourceView(1, m_instanceDatasBuffer.GetResource()->GetGPUVirtualAddress());
					cmdBuf->SetGraphicsRootShaderResourceView(2, m_materialDatasBuffer.GetResource()->GetGPUVirtualAddress());
					cmdBuf->SetGraphicsRootShaderResourceView(3, m_animUpdator.GetResultSkeletonBuffer().GetResource()->GetGPUVirtualAddress());
					
					ID3D12DescriptorHeap* srvDescHeaps[] = { m_textureDescriptorHeap };
					cmdBuf->SetDescriptorHeaps(_countof(srvDescHeaps), srvDescHeaps);
					
					uint32_t bindIndex = 4;
					CD3DX12_GPU_DESCRIPTOR_HANDLE handle(m_textureDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
					for (auto cur = m_textures.begin(); cur != m_textures.end(); cur++)
					{					
						cmdBuf->SetGraphicsRootDescriptorTable(bindIndex, handle);
						handle.Offset(1, gDX12DeviceRes->GetCbvSrvUavDiscriptorSize());
						bindIndex++;
					}

					for (uint32_t j = 0; j < geometry->GetMeshCount(); j++)
					{
						SimpleMeshData* mesh = geometry->GetMesh(j);
						if (mesh != nullptr)
						{
							cmdBuf->IASetVertexBuffers(0, 1, &mesh->GetVertexBuffer().GetVertexBufferView());
							cmdBuf->IASetIndexBuffer(&mesh->GetIndexBuffer().GetIndexBufferView());
							cmdBuf->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
							
							cmdBuf->DrawIndexedInstanced
							(
								mesh->GetIndexBuffer().GetCount(),
								m_renderObj->GetInstanceCount(),
								0,
								0,
								0
							);
						}
					}

					cmdBuf->ResourceBarrier
					(
						1,
						&CD3DX12_RESOURCE_BARRIER::Transition
						(
							gDX12DeviceRes->GetSwapChain().GetBackBuffer(i),
							D3D12_RESOURCE_STATE_RENDER_TARGET,
							D3D12_RESOURCE_STATE_PRESENT
						)
					);

					cmdBuffer->End();
				}
			}
		}
	}

	return true;
}

const wchar_t* RenderItem_MeshShaderInstancing::MESHSHADER_INSTANCING_AS_FILE_PATH = L"../Resources/Shaders/MeshShaderInstancing/MSInstancing_AS.cso";
const wchar_t* RenderItem_MeshShaderInstancing::MESHSHADER_INSTANCING_MS_FILE_PATH = L"../Resources/Shaders/MeshShaderInstancing/MSInstancing_MS.cso";
const wchar_t* RenderItem_MeshShaderInstancing::MESHSHADER_INSTANCING_PS_FILE_PATH = L"../Resources/Shaders/MeshShaderInstancing/MSInstancing_PS.cso";

bool RenderItem_MeshShaderInstancing::CreateShader()
{
	m_shader = new SimpleShader(static_cast<uint8_t>(EShaderStageType::AMPLIFICATION_SHADER) | static_cast<uint8_t>(EShaderStageType::MESH_SHADER) | static_cast<uint8_t>(EShaderStageType::PIXEL_SHADER));
	if (m_shader != nullptr)
	{
		m_shader->LoadCompiledShader(EShaderStageType::AMPLIFICATION_SHADER, MESHSHADER_INSTANCING_AS_FILE_PATH);
		m_shader->LoadCompiledShader(EShaderStageType::MESH_SHADER, MESHSHADER_INSTANCING_MS_FILE_PATH);
		m_shader->LoadCompiledShader(EShaderStageType::PIXEL_SHADER, MESHSHADER_INSTANCING_PS_FILE_PATH);
		return true;
	}
	return false;
}

bool RenderItem_MeshShaderInstancing::CreateRootSignature()
{
	std::vector<CD3DX12_DESCRIPTOR_RANGE> textureRanges(m_textures.size());
	for (uint32_t i = 0; i < textureRanges.size(); i++)
	{
		textureRanges[i].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, i, 0);
	}

	std::vector<CD3DX12_ROOT_PARAMETER> slotRootParameterList;
	CD3DX12_ROOT_PARAMETER globalConstantSlotRootParameter = {};
	globalConstantSlotRootParameter.InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL);
	slotRootParameterList.push_back(globalConstantSlotRootParameter);

	CD3DX12_ROOT_PARAMETER drawParamSlotRootParameter = {};
	drawParamSlotRootParameter.InitAsConstants(5, 1, 0, D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL);
	slotRootParameterList.push_back(drawParamSlotRootParameter);

	CD3DX12_ROOT_PARAMETER vertexBufferSlotRootParameter = {};
	vertexBufferSlotRootParameter.InitAsShaderResourceView(0, 1, D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_MESH);
	slotRootParameterList.push_back(vertexBufferSlotRootParameter);

	CD3DX12_ROOT_PARAMETER meshletBufferSlotRootParameter = {};
	meshletBufferSlotRootParameter.InitAsShaderResourceView(1, 1, D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_MESH);
	slotRootParameterList.push_back(meshletBufferSlotRootParameter);

	CD3DX12_ROOT_PARAMETER locVertIdxBufferSlotRootParameter = {};
	locVertIdxBufferSlotRootParameter.InitAsShaderResourceView(2, 1, D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_MESH);
	slotRootParameterList.push_back(locVertIdxBufferSlotRootParameter);

	CD3DX12_ROOT_PARAMETER locPrimIdxBufferSlotRootParameter = {};
	locPrimIdxBufferSlotRootParameter.InitAsShaderResourceView(3, 1, D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_MESH);
	slotRootParameterList.push_back(locPrimIdxBufferSlotRootParameter);

	CD3DX12_ROOT_PARAMETER instanceDataBufferSlotRootParameter = {};
	instanceDataBufferSlotRootParameter.InitAsShaderResourceView(4, 1, D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_MESH);
	slotRootParameterList.push_back(instanceDataBufferSlotRootParameter);

	CD3DX12_ROOT_PARAMETER materialDataBufferSlotRootParameter = {};
	materialDataBufferSlotRootParameter.InitAsShaderResourceView(5, 1, D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL);
	slotRootParameterList.push_back(materialDataBufferSlotRootParameter);

	CD3DX12_ROOT_PARAMETER skeletonDataBufferSlotRootParameter = {};
	skeletonDataBufferSlotRootParameter.InitAsShaderResourceView(6, 1, D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_MESH);
	slotRootParameterList.push_back(skeletonDataBufferSlotRootParameter);

	for (int i = 0; i < textureRanges.size(); i++)
	{
		CD3DX12_ROOT_PARAMETER textureSlotRootParameter = {};
		textureSlotRootParameter.InitAsDescriptorTable(1, &textureRanges[i], D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_PIXEL);
		slotRootParameterList.push_back(textureSlotRootParameter);
	}

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc
	(
		static_cast<uint32_t>(slotRootParameterList.size()),
		slotRootParameterList.data(),
		1,
		&m_linearWrapSampler,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
	);

	ID3DBlob* serializedRootSig = nullptr;
	ID3DBlob* errorBlob = nullptr;

	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &serializedRootSig, &errorBlob);
	if (errorBlob != nullptr)
	{
		REPORT(EReportType::REPORT_TYPE_ERROR, "Root signature serialize failed");
		return false;
	}

	HRESULT res = gLogicalDevice->CreateRootSignature(0, serializedRootSig->GetBufferPointer(), serializedRootSig->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature));
	if (FAILED(res))
	{
		REPORT(EReportType::REPORT_TYPE_ERROR, "Root signature create failed.");
		return false;
	}
	return true;
}

bool RenderItem_MeshShaderInstancing::CreatePSO()
{
	D3DX12_MESH_SHADER_PIPELINE_STATE_DESC meshShaderPsoDesc = {};
	meshShaderPsoDesc.pRootSignature = m_rootSignature;
	meshShaderPsoDesc.AS =
	{
		reinterpret_cast<BYTE*>(m_shader->GetShader(EShaderStageType::AMPLIFICATION_SHADER)->GetBufferPointer()),
		m_shader->GetShader(EShaderStageType::AMPLIFICATION_SHADER)->GetBufferSize()
	};
	meshShaderPsoDesc.MS =
	{
		reinterpret_cast<BYTE*>(m_shader->GetShader(EShaderStageType::MESH_SHADER)->GetBufferPointer()),
		m_shader->GetShader(EShaderStageType::MESH_SHADER)->GetBufferSize()
	};
	meshShaderPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(m_shader->GetShader(EShaderStageType::PIXEL_SHADER)->GetBufferPointer()),
		m_shader->GetShader(EShaderStageType::PIXEL_SHADER)->GetBufferSize()
	};
	meshShaderPsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	meshShaderPsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	meshShaderPsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	meshShaderPsoDesc.SampleMask = UINT_MAX;
	meshShaderPsoDesc.NumRenderTargets = 1;
	meshShaderPsoDesc.RTVFormats[0] = gDX12DeviceRes->GetSwapChain().GetBackBufferFormat();
	meshShaderPsoDesc.SampleDesc.Count = gDX12DeviceRes->GetMsaaState() ? 4 : 1;
	meshShaderPsoDesc.SampleDesc.Quality = gDX12DeviceRes->GetMsaaState() ? (gDX12DeviceRes->GetMsaaQuality() - 1) : 0;
	meshShaderPsoDesc.DSVFormat = gDX12DeviceRes->GetSwapChain().GetDepthStencilBufferFormat();

	auto psoStream = CD3DX12_PIPELINE_MESH_STATE_STREAM(meshShaderPsoDesc);

	D3D12_PIPELINE_STATE_STREAM_DESC streamDesc;
	streamDesc.pPipelineStateSubobjectStream = &psoStream;
	streamDesc.SizeInBytes = sizeof(psoStream);

	HRESULT res = gLogicalDevice->CreatePipelineState(&streamDesc, IID_PPV_ARGS(&m_pso));
	if (FAILED(res))
	{
		return false;
	}

	return true;
}

bool RenderItem_MeshShaderInstancing::CreateCommandBuffer()
{
	if (m_renderObj != nullptr)
	{
		SimpleGeometry* geometry = m_renderObj->GetGeometry();
		if (geometry != nullptr)
		{
			for (uint32_t i = 0; i < m_renderCommandBuffers.GetCommandBufferCount(); i++)
			{
				CommandBuffer* cmdBuffer = m_renderCommandBuffers.GetCommandBuffer(i);
				if (cmdBuffer != nullptr && cmdBuffer->Begin(m_pso))
				{
					ID3D12GraphicsCommandList6* cmdBuf = static_cast<ID3D12GraphicsCommandList6*>(cmdBuffer->GetCommandBuffer());
					if (cmdBuf != nullptr)
					{
						cmdBuf->RSSetViewports(1, &gDX12DeviceRes->GetViewport());
						cmdBuf->RSSetScissorRects(1, &gDX12DeviceRes->GetScissorRect());

						cmdBuf->ResourceBarrier
						(
							1,
							&CD3DX12_RESOURCE_BARRIER::Transition
							(
								gDX12DeviceRes->GetSwapChain().GetBackBuffer(i),
								D3D12_RESOURCE_STATE_PRESENT,
								D3D12_RESOURCE_STATE_RENDER_TARGET
							)
						);

						cmdBuf->OMSetRenderTargets(1, &gDX12DeviceRes->GetSwapChain().GetRtvHandle(i), true, &gDX12DeviceRes->GetSwapChain().GetDsvHandle());
						cmdBuf->ClearRenderTargetView(gDX12DeviceRes->GetSwapChain().GetRtvHandle(i), Colors::Black, 0, nullptr);
						cmdBuf->ClearDepthStencilView(gDX12DeviceRes->GetSwapChain().GetDsvHandle(),
							D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
							1.0f, 0, 0, nullptr);

						cmdBuf->SetGraphicsRootSignature(m_rootSignature);
						cmdBuf->SetGraphicsRootConstantBufferView(0, m_globalConstntBuffer->GetResource()->GetGPUVirtualAddress());
						cmdBuf->SetGraphicsRootShaderResourceView(6, m_instanceDatasBuffer.GetResource()->GetGPUVirtualAddress());
						cmdBuf->SetGraphicsRootShaderResourceView(7, m_materialDatasBuffer.GetResource()->GetGPUVirtualAddress());
						cmdBuf->SetGraphicsRootShaderResourceView(8, m_animUpdator.GetResultSkeletonBuffer().GetResource()->GetGPUVirtualAddress());

						ID3D12DescriptorHeap* srvDescHeaps[] = { m_textureDescriptorHeap };
						cmdBuf->SetDescriptorHeaps(_countof(srvDescHeaps), srvDescHeaps);

						uint32_t bindIndex = 9;
						CD3DX12_GPU_DESCRIPTOR_HANDLE handle(m_textureDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
						for (auto cur = m_textures.begin(); cur != m_textures.end(); cur++)
						{
							cmdBuf->SetGraphicsRootDescriptorTable(bindIndex, handle);
							handle.Offset(1, gDX12DeviceRes->GetCbvSrvUavDiscriptorSize());
							bindIndex++;
						}

						for (uint32_t j = 0; j < geometry->GetMeshCount(); j++)
						{
							SimpleMeshData* mesh = geometry->GetMesh(j);
							if (mesh != nullptr)
							{
								uint32_t meshletCount = mesh->GetMeshletInfo().MeshletCount;
								uint32_t lastMeshletVertIndexCount = mesh->GetMeshletInfo().LastMeshletVertIdxCount;
								uint32_t lastMeshletPrimCount = mesh->GetMeshletInfo().LastMeshletPrimCount;
								cmdBuf->SetGraphicsRoot32BitConstant(1, meshletCount, 2);
								cmdBuf->SetGraphicsRoot32BitConstant(1, lastMeshletVertIndexCount, 3);
								cmdBuf->SetGraphicsRoot32BitConstant(1, lastMeshletPrimCount, 4);

								cmdBuf->SetGraphicsRootShaderResourceView(2, mesh->GetVertexBuffer().GetResource()->GetGPUVirtualAddress());
								cmdBuf->SetGraphicsRootShaderResourceView(3, mesh->GetMeshletBuffer().GetResource()->GetGPUVirtualAddress());
								cmdBuf->SetGraphicsRootShaderResourceView(4, mesh->GetVertLocIdxBuffer().GetResource()->GetGPUVirtualAddress());
								cmdBuf->SetGraphicsRootShaderResourceView(5, mesh->GetPrimLocIdxBuffer().GetResource()->GetGPUVirtualAddress());

								int numInstances = m_renderObj->GetInstanceCount();
								uint32_t dispatchCount = DivRoundUp(numInstances, MAX_DISPATCH_GROUP_COUNT);
								for (uint32_t k = 0; k < dispatchCount; ++k)
								{
									uint32_t numCurDispatchInstances = min(numInstances - MAX_DISPATCH_GROUP_COUNT * k, MAX_DISPATCH_GROUP_COUNT);
									uint32_t offset = max(MAX_DISPATCH_GROUP_COUNT * k, 0);
									uint32_t dispatchThreadCount = min(numInstances - offset, MAX_DISPATCH_GROUP_COUNT);
									cmdBuf->SetGraphicsRoot32BitConstant(1, numCurDispatchInstances, 0);
									cmdBuf->SetGraphicsRoot32BitConstant(1, offset, 1);
									cmdBuf->DispatchMesh(DivRoundUp(dispatchThreadCount, MAX_INSTANCE_COUNT_PER_AS), 1, 1);
								}
							}
						}

						cmdBuf->ResourceBarrier
						(
							1,
							&CD3DX12_RESOURCE_BARRIER::Transition
							(
								gDX12DeviceRes->GetSwapChain().GetBackBuffer(i),
								D3D12_RESOURCE_STATE_RENDER_TARGET,
								D3D12_RESOURCE_STATE_PRESENT
							)
						);
					}
					cmdBuffer->End();
				}
			}
		}
	}

	return true;
}