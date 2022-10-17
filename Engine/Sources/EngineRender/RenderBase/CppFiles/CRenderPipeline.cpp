#include "../Headers/CRenderPipeline.h"
#include "../../../EngineBase/Headers/CManager.h"
#include "../../../EngineGame/Headers/CGameObject.h"
#include "../../../EngineGame/Headers/CCamera.h"
#include "../../../EngineGame/Headers/CLightType.h"
#include "../Headers/CRenderDevice.h"
#include "../Headers/CGPUQuery.h"
#include "../Headers/CGPUQueryManager.h"
#include "../Headers/CGPUCulling.h"
#include "../../AssetsManager/Headers/CMeshManager.h"
#include "../Headers/CMeshRenderer.h"
#include "../../AssetsManager/Headers/CShader.h"
#include "../../AssetsManager/Headers/CShaderManager.h"
#include "../../AssetsManager/Headers/CTextureType.h"
#include "../../AssetsManager/Headers/CTextureManager.h"
#include "../../../EngineGame/Headers/CSkyBox.h"
#include "../../RenderFeatures/Headers/CGTAOPass.h"
#include "../../RenderFeatures/Headers/CHZBPass.h"

#include "../../../Development/Headers/CDebugScreen.h"
#include "../../../Development/Headers/CGPUProfiler.h"

CTexture2D*							CRenderPipeline::m_DefaultTexture[CustomStruct::CEngineDefaultTexture2DEnum::ENGINE_DEFAULT_TEXTURE2D_COUNT] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
std::shared_ptr<CPixelShader>		CRenderPipeline::m_DefaultEmptyPS		= nullptr;
std::shared_ptr<CVertexShader>		CRenderPipeline::m_FullScreenPolygonVS	= nullptr;
std::shared_ptr<CPixelShader>		CRenderPipeline::m_ScreenPolygonShader	= nullptr;
std::shared_ptr<CPixelShader>		CRenderPipeline::m_DirectLightShader	= nullptr;
std::shared_ptr<CSkyBox>			CRenderPipeline::m_SkyBox				= nullptr;
std::shared_ptr<CGPUCulling>		CRenderPipeline::m_GPUCulling			= nullptr;
std::shared_ptr<CGTAOPass>			CRenderPipeline::m_GTAOPass				= nullptr;
std::shared_ptr<CHZBPass>			CRenderPipeline::m_HZBPass				= nullptr;
std::shared_ptr<CDebugScreen>		CRenderPipeline::m_DebugScreen			= nullptr;

CRenderPipeline::CRenderPipeline()
{
	for (INT i = 0; i < CustomStruct::CEngineDefaultTexture2DEnum::ENGINE_DEFAULT_TEXTURE2D_COUNT; i++)
	{
		if (CRenderPipeline::m_DefaultTexture[i] == NULL)
		{
			CRenderPipeline::m_DefaultTexture[i] = CTextureManager::LoadTexture2D(CustomStruct::CEngineDefaultDefines::GetDefaultTexturePath(i), FALSE);
		}
	}
	if (CRenderPipeline::m_DefaultEmptyPS == nullptr)
	{
		CRenderPipeline::m_DefaultEmptyPS = CShaderManager::LoadPixelShader(ENGINE_SHADER_EMPTY_PS);
	}
	if (CRenderPipeline::m_FullScreenPolygonVS == nullptr)
	{
		CustomStruct::CRenderInputLayoutDesc desc[2u] = {
			CustomStruct::CRenderInputLayoutDesc(CustomStruct::CRenderShaderSemantic::SHADER_SEMANTIC_POSITION),
			CustomStruct::CRenderInputLayoutDesc(CustomStruct::CRenderShaderSemantic::SHADER_SEMANTIC_TEXCOORD) };
		CRenderPipeline::m_FullScreenPolygonVS = CShaderManager::LoadVertexShader(ENGINE_SHADER_SCREEN_POLYGON_2D_VS, desc, 2u);
	}
	if (CRenderPipeline::m_ScreenPolygonShader == nullptr)
	{
		CRenderPipeline::m_ScreenPolygonShader = CShaderManager::LoadPixelShader(ENGINE_SHADER_SCREEN_POLYGON_2D_PS);
	}
	if (CRenderPipeline::m_DirectLightShader == nullptr)
	{
		CRenderPipeline::m_DirectLightShader = CShaderManager::LoadPixelShader(ENGINE_SHADER_DIRECT_LIGHT_PS);
	}
	if (CRenderPipeline::m_SkyBox == nullptr)
	{
		CRenderPipeline::m_SkyBox = std::shared_ptr<CSkyBox>(new CSkyBox());
	}
	if (CRenderPipeline::m_GPUCulling == nullptr)
	{
		CRenderPipeline::m_GPUCulling = std::shared_ptr<CGPUCulling>(new CGPUCulling());
	}
	if (CRenderPipeline::m_GTAOPass == nullptr)
	{
		CRenderPipeline::m_GTAOPass = std::shared_ptr<CGTAOPass>(new CGTAOPass());
	}
	if (CRenderPipeline::m_HZBPass == nullptr)
	{
		CRenderPipeline::m_HZBPass = std::shared_ptr<CHZBPass>(new CHZBPass());
	}
	if (CRenderPipeline::m_DebugScreen == nullptr)
	{
		CRenderPipeline::m_DebugScreen = std::shared_ptr<CDebugScreen>(new CDebugScreen());
	}

	m_CurrentScene		= NULL;
	m_GlobalBufferSize	= CustomType::Vector2Int(0, 0);
	m_FullScreenPolygon	= NULL;
}
CRenderPipeline::~CRenderPipeline()
{
}
void CRenderPipeline::Init(const CScene* scene, const CustomType::Vector2Int& bufferSize)
{
	m_CurrentScene = scene;
	m_GlobalBufferSize = bufferSize;
	m_ShadowBufferSize = CustomType::Vector2Int(2048, 2048);
	m_GlobalCullingInfo.Distance = ENGINE_DEFAULT_CULLING_DISTANCE;
	m_GlobalCullingInfo.ClipOffset = ENGINE_DEFAULT_CULLING_OFFSET;

	{
		CustomType::Vector4Int fullScreenSize(0, 0, bufferSize.X(), bufferSize.Y());
		CustomStruct::CRenderInputLayoutDesc desc[2u] = {
			CustomStruct::CRenderInputLayoutDesc(CustomStruct::CRenderShaderSemantic::SHADER_SEMANTIC_POSITION),
			CustomStruct::CRenderInputLayoutDesc(CustomStruct::CRenderShaderSemantic::SHADER_SEMANTIC_TEXCOORD) };
		m_FullScreenPolygon = CMeshManager::LoadPolygon2D(fullScreenSize, desc, 2u);
	}

	for (INT i = 0; i < CScene::SceneLayout::LAYOUT_COUNT; i++)
	{
		if (m_CurrentScenePrimitives[i].size() != 0)
		{
			m_CurrentScenePrimitives[i].clear();
		}
	}
	if (m_CurrentCPUCullingPrimitives.size() != 0)
	{
		m_CurrentCPUCullingPrimitives.clear();
	}

	{
		CRenderDevice::CreateBuffer(
			m_RenderPerFrameInfo.PerFrameBuffer,
			CustomStruct::CRenderBufferDesc(
				sizeof(CustomStruct::CShaderGlobalPerFrame),
				CustomStruct::CRenderBindFlag::BIND_CONSTANT_BUFFER,
				sizeof(FLOAT)));
		CRenderDevice::CreateBuffer(
			m_RenderLightDataInfo.LightDataBuffer,
			CustomStruct::CRenderBufferDesc(
				sizeof(CustomStruct::CShaderGlobalLightData),
				CustomStruct::CRenderBindFlag::BIND_CONSTANT_BUFFER,
				sizeof(FLOAT)));
	}

	{
		UINT bufferWidth = static_cast<UINT>(bufferSize.X());
		UINT bufferHeight = static_cast<UINT>(bufferSize.Y());
		for (UINT i = 0u; i < CRenderPipeline::GEOMETRY_BUFFER_COUNT; i++)
		{
			CRenderDevice::CreateRenderTexture2D(
				m_GBuffer[i],
				CustomStruct::CRenderTextureDesc(
					bufferWidth,
					bufferHeight,
					CustomStruct::CRenderBindFlag::BIND_RTV_SRV,
					CustomStruct::CRenderFormat::FORMAT_R16G16B16A16_FLOAT));
		}
		CRenderDevice::CreateRenderTexture2D(
			m_SceneColor,
			CustomStruct::CRenderTextureDesc(
				bufferWidth,
				bufferHeight,
				CustomStruct::CRenderBindFlag::BIND_RTV_SRV,
				CustomStruct::CRenderFormat::FORMAT_R11G11B10_FLOAT));
		CustomStruct::CRenderFormat format = CustomStruct::CRenderFormat::FORMAT_UNKNOWN;
		CRenderDevice::CreateRenderTexture2D(
			m_SceneDepth,
			CustomStruct::CRenderTextureDesc(
				bufferWidth,
				bufferHeight,
				CustomStruct::CRenderBindFlag::BIND_NONE,
				format, &format, &format, &format,
				32u));
		for (INT i = 0; i < 2; i++)
		{
			CRenderDevice::CreateRenderTexture2D(
				m_PostProcessColor[i],
				CustomStruct::CRenderTextureDesc(
					bufferWidth,
					bufferHeight,
					CustomStruct::CRenderBindFlag::BIND_RTV_SRV,
					CustomStruct::CRenderFormat::FORMAT_R11G11B10_FLOAT));
		}
	}

	{
		CRenderDevice::CreateRasterizerState(m_PipelineRS,
			CustomStruct::CRenderRasterizerState(
				CustomStruct::CRenderCullMode::CULL_BACK,
				CustomStruct::CRenderFillMode::FILL_SOLID));

		std::vector<CustomStruct::CRenderBlendState> blendStates;
		blendStates.resize(1);
		blendStates[0] = CustomStruct::CRenderBlendState();
		CRenderDevice::CreateBlendState(m_ShadowSkyForwardPrePassBS, blendStates);
		blendStates.resize(4);
		blendStates[0] = CustomStruct::CRenderBlendState();
		blendStates[1] = CustomStruct::CRenderBlendState();
		blendStates[2] = CustomStruct::CRenderBlendState();
		blendStates[3] = CustomStruct::CRenderBlendState();
		CRenderDevice::CreateBlendState(m_GBufferPassBS, blendStates);
		blendStates.resize(1);
		blendStates[0] = CustomStruct::CRenderBlendState(
			CustomStruct::CRenderBlendOption::BLEND_ONE,
			CustomStruct::CRenderBlendOption::BLEND_ONE,
			CustomStruct::CRenderBlendOperation::BLEND_OP_ADD,
			CustomStruct::CRenderBlendOption::BLEND_ONE,
			CustomStruct::CRenderBlendOption::BLEND_ONE,
			CustomStruct::CRenderBlendOperation::BLEND_OP_MAX);
		CRenderDevice::CreateBlendState(m_DirectLightPassBS, blendStates);
		blendStates.resize(1);
		blendStates[0] = CustomStruct::CRenderBlendState(
			CustomStruct::CRenderBlendOption::BLEND_SRC_ALPHA,
			CustomStruct::CRenderBlendOption::BLEND_INV_SRC_ALPHA,
			CustomStruct::CRenderBlendOperation::BLEND_OP_ADD,
			CustomStruct::CRenderBlendOption::BLEND_ONE,
			CustomStruct::CRenderBlendOption::BLEND_ONE,
			CustomStruct::CRenderBlendOperation::BLEND_OP_MAX);
		CRenderDevice::CreateBlendState(m_TransparentPassBS, blendStates);

		CRenderDevice::CreateDepthStencilState(m_ShadowPrePassDSS,
			CustomStruct::CRenderDepthState(
				CustomStruct::CRenderComparisonFunction::COMPARISON_LESS));
		CRenderDevice::CreateDepthStencilState(m_GBufferForwardPassDSS,
			CustomStruct::CRenderDepthState(
				CustomStruct::CRenderComparisonFunction::COMPARISON_LESS_EQUAL));
		CRenderDevice::CreateDepthStencilState(m_DirectLightPassDSS,
			CustomStruct::CRenderDepthState(
				CustomStruct::CRenderComparisonFunction::COMPARISON_ALWAYS));
	}

	{
		CRenderDevice::CreateSamplerState(m_PipelineSampler[0],
			CustomStruct::CRenderSamplerState(
				CustomStruct::CRenderFilter::FILTER_POINT,
				CustomStruct::CRenderTextureAddressMode::TEXTURE_ADDRESS_CLAMP,
				CustomStruct::CRenderTextureAddressMode::TEXTURE_ADDRESS_CLAMP,
				CustomStruct::CRenderTextureAddressMode::TEXTURE_ADDRESS_CLAMP));
		CRenderDevice::CreateSamplerState(m_PipelineSampler[1],
			CustomStruct::CRenderSamplerState(
				CustomStruct::CRenderFilter::FILTER_POINT,
				CustomStruct::CRenderTextureAddressMode::TEXTURE_ADDRESS_WRAP,
				CustomStruct::CRenderTextureAddressMode::TEXTURE_ADDRESS_WRAP,
				CustomStruct::CRenderTextureAddressMode::TEXTURE_ADDRESS_WRAP));
		CRenderDevice::CreateSamplerState(m_PipelineSampler[2],
			CustomStruct::CRenderSamplerState(
				CustomStruct::CRenderFilter::FILTER_LINEAR,
				CustomStruct::CRenderTextureAddressMode::TEXTURE_ADDRESS_CLAMP,
				CustomStruct::CRenderTextureAddressMode::TEXTURE_ADDRESS_CLAMP,
				CustomStruct::CRenderTextureAddressMode::TEXTURE_ADDRESS_CLAMP));
		CRenderDevice::CreateSamplerState(m_PipelineSampler[3],
			CustomStruct::CRenderSamplerState(
				CustomStruct::CRenderFilter::FILTER_LINEAR,
				CustomStruct::CRenderTextureAddressMode::TEXTURE_ADDRESS_WRAP,
				CustomStruct::CRenderTextureAddressMode::TEXTURE_ADDRESS_WRAP,
				CustomStruct::CRenderTextureAddressMode::TEXTURE_ADDRESS_WRAP));
	}
}
void CRenderPipeline::PostInit()
{
	if (m_CurrentScene == NULL)
		return;

	{
		std::vector<CLightBase*> rawSceneLights = m_CurrentScene->GetGameObjectAll<CLightBase>(CScene::SceneLayout::LAYOUT_LIGHT);
		CLightBase* sceneMainLight = NULL;
		for (INT i = 0; i < rawSceneLights.size(); i++)
		{
			if (rawSceneLights[i]->IsActive())
			{
				sceneMainLight = rawSceneLights[i];
				break;
			}
		}
		UINT bufferWidth = 1u;
		UINT bufferHeight = 1u;
		if (sceneMainLight)
		{
			bufferWidth = static_cast<UINT>(m_ShadowBufferSize.X());
			bufferHeight = static_cast<UINT>(m_ShadowBufferSize.Y());
		}
		CustomStruct::CRenderFormat format = CustomStruct::CRenderFormat::FORMAT_UNKNOWN;
		CRenderDevice::CreateRenderTexture2D(
			m_ShadowBuffer,
			CustomStruct::CRenderTextureDesc(
				bufferWidth,
				bufferHeight,
				CustomStruct::CRenderBindFlag::BIND_NONE,
				format, &format, &format, &format,
				32u));
	}

	{
		std::vector<CCamera*> rawSceneCameras = m_CurrentScene->GetGameObjectAll<CCamera>(CScene::SceneLayout::LAYOUT_CAMERA);
		CCamera* sceneMainCamera = NULL;
		for (INT i = 0; i < rawSceneCameras.size(); i++)
		{
			if (rawSceneCameras[i]->IsActive())
			{
				sceneMainCamera = rawSceneCameras[i];
				break;
			}
		}

		m_FrameIndex = 0u;

		m_SkyBox->Init(m_GlobalBufferSize, CSkyBox::SkyBoxInfo(1000000.f));

		m_GTAOPass->Init(sceneMainCamera, m_GlobalBufferSize);
		m_HZBPass->Init(m_GlobalBufferSize);
		m_GPUCulling->Init(m_HZBPass, 1u, 50u);
		m_DebugScreen->Init(m_GlobalBufferSize);
	}
}
void CRenderPipeline::Uninit()
{
	for (INT i = 0; i < CScene::SceneLayout::LAYOUT_COUNT; i++)
	{
		if (m_CurrentScenePrimitives[i].size() != 0)
		{
			m_CurrentScenePrimitives[i].clear();
		}
	}
	if (m_CurrentCPUCullingPrimitives.size() != 0)
	{
		m_CurrentCPUCullingPrimitives.clear();
	}

	m_DebugScreen->Uninit();
	m_GTAOPass->Uninit();
	m_HZBPass->Uninit();
	m_GPUCulling->Uninit();

	m_SkyBox->Uninit();
}
void CRenderPipeline::PostUpdate()
{
	if (m_CurrentScene == NULL)
		return;

	{
		m_FrameIndex += 1u;

		for (INT i = 0; i < CScene::SceneLayout::LAYOUT_COUNT; i++)
		{
			if (m_CurrentScenePrimitives[i].size() != 0)
			{
				m_CurrentScenePrimitives[i].clear();
			}
		}
		if (m_CurrentCPUCullingPrimitives.size() != 0)
		{
			m_CurrentCPUCullingPrimitives.clear();
		}
		for (auto obj : m_CurrentScene->m_GameObject[CScene::SceneLayout::LAYOUT_CAMERA])
		{
			if (obj.second->IsActive())
			{
				m_CurrentScenePrimitives[CScene::SceneLayout::LAYOUT_CAMERA].push_back(obj.second);
				break;
			}
		}
		for (auto obj : m_CurrentScene->m_GameObject[CScene::SceneLayout::LAYOUT_LIGHT])
		{
			if (obj.second->IsActive())
			{
				m_CurrentScenePrimitives[CScene::SceneLayout::LAYOUT_LIGHT].push_back(obj.second);
			}
		}
		for (auto obj : m_CurrentScene->m_GameObject[CScene::SceneLayout::LAYOUT_TERRAIN])
		{
			if (obj.second->IsActive() && obj.second->GetMeshRenderer())
			{
				m_CurrentScenePrimitives[CScene::SceneLayout::LAYOUT_TERRAIN].push_back(obj.second);
			}
		}

		{
			std::vector<CGameObject*> objectLayer;
			for (INT i = CScene::SceneLayout::LAYOUT_OPAQUE; i <= CScene::SceneLayout::LAYOUT_TRANSPARENT; i++)
			{
				for (auto obj : m_CurrentScene->m_GameObject[i])
				{
					if (obj.second->IsActive() && obj.second->GetMeshRenderer())
					{
						objectLayer.push_back(obj.second);
					}
				}
			}
			PrepareCameraCullingInfo(m_GlobalCullingInfo, reinterpret_cast<CCamera*>((m_CurrentScenePrimitives[CScene::SceneLayout::LAYOUT_CAMERA])[0]));
			CRenderPipeline::Culling(m_CurrentCPUCullingPrimitives, m_GlobalCullingInfo, objectLayer);
			std::vector<BOOL> objectGPUCullingResult;
			m_GPUCulling->ReadBackAndPrepareCullingResult(m_FrameIndex, m_CurrentCPUCullingPrimitives, objectGPUCullingResult);
			for (UINT i = 0; i < m_CurrentCPUCullingPrimitives.size(); i++)
			{
				if (objectGPUCullingResult[i] == TRUE)
				{
					if (m_CurrentCPUCullingPrimitives[i]->GetMeshRenderer()->GetRenderType() == CMeshRenderer::RenderTypeEnum::RENDER_TYPE_OPAQUE
						|| m_CurrentCPUCullingPrimitives[i]->GetMeshRenderer()->GetRenderType() == CMeshRenderer::RenderTypeEnum::RENDER_TYPE_OPAQUE_FORWARD)
					{
						m_CurrentScenePrimitives[CScene::SceneLayout::LAYOUT_OPAQUE].push_back(m_CurrentCPUCullingPrimitives[i]);
					}
					else if (m_CurrentCPUCullingPrimitives[i]->GetMeshRenderer()->GetRenderType() == CMeshRenderer::RenderTypeEnum::RENDER_TYPE_TRANSPARENT_FORWARD)
					{
						m_CurrentScenePrimitives[CScene::SceneLayout::LAYOUT_TRANSPARENT].push_back(m_CurrentCPUCullingPrimitives[i]);
					}
				}
			}
		}
	}

	m_SkyBox->Update();

	m_GTAOPass->Update();
	m_HZBPass->Update();
	m_GPUCulling->Update(m_FrameIndex);
	m_DebugScreen->Update();
}
void CRenderPipeline::Render()
{
	if (m_CurrentScene == NULL)
		return;

	{
		CRenderDevice::ClearRenderTargetView(m_SceneColor.RenderTargetView);
		for (UINT i = 0u; i < CRenderPipeline::GEOMETRY_BUFFER_COUNT; i++)
		{
			CRenderDevice::ClearRenderTargetView(m_GBuffer[i].RenderTargetView);
		}
		for (UINT i = 0u; i < 2u; i++)
		{
			CRenderDevice::ClearRenderTargetView(m_PostProcessColor[i].RenderTargetView);
		}
		CRenderDevice::ClearDepthStencilView(m_SceneDepth.DepthStencilView);
		CRenderDevice::ClearDepthStencilView(m_ShadowBuffer.DepthStencilView, CustomStruct::CRenderClearDepthStencilFlag::CLEAR_DEPTH);
		CRenderDevice::ClearFinalOutput();

		CRenderDevice::BindVSSamplerStates(m_PipelineSampler, ENGINE_SAMPLER_ALL_START_SLOT, 4u);
		CRenderDevice::BindPSSamplerStates(m_PipelineSampler, ENGINE_SAMPLER_ALL_START_SLOT, 4u);
		CRenderDevice::BindCSSamplerStates(m_PipelineSampler, ENGINE_SAMPLER_ALL_START_SLOT, 4u);
	}

	for (INT i = 0; i < (m_CurrentScenePrimitives[CScene::SceneLayout::LAYOUT_CAMERA]).size(); i++)
	{
		{
			CCamera* camera = reinterpret_cast<CCamera*>((m_CurrentScenePrimitives[CScene::SceneLayout::LAYOUT_CAMERA])[i]);
			if (!camera)
				continue;

			PreparePerFrameRender(camera);
			PrepareLightDataRender();

			CRenderDevice::UploadBuffer(m_RenderPerFrameInfo.PerFrameBuffer, &(m_RenderPerFrameInfo.PerFrameData));
			CRenderDevice::UploadBuffer(m_RenderLightDataInfo.LightDataBuffer, &(m_RenderLightDataInfo.LightData));

			CRenderDevice::BindVSConstantBuffer(m_RenderPerFrameInfo.PerFrameBuffer, ENGINE_CONSTANT_BUFFER_PER_FRAME_START_SLOT);
			CRenderDevice::BindPSConstantBuffer(m_RenderPerFrameInfo.PerFrameBuffer, ENGINE_CONSTANT_BUFFER_PER_FRAME_START_SLOT);
			CRenderDevice::BindCSConstantBuffer(m_RenderPerFrameInfo.PerFrameBuffer, ENGINE_CONSTANT_BUFFER_PER_FRAME_START_SLOT);

			CRenderDevice::BindVSConstantBuffer(m_RenderLightDataInfo.LightDataBuffer, ENGINE_CONSTANT_BUFFER_LIGHT_DATA_START_SLOT);
			CRenderDevice::BindPSConstantBuffer(m_RenderLightDataInfo.LightDataBuffer, ENGINE_CONSTANT_BUFFER_LIGHT_DATA_START_SLOT);
			CRenderDevice::BindCSConstantBuffer(m_RenderLightDataInfo.LightDataBuffer, ENGINE_CONSTANT_BUFFER_LIGHT_DATA_START_SLOT);

			CRenderDevice::SetViewport(camera->GetViewport());
			CRenderDevice::SetRasterizerState(m_PipelineRS);
			CRenderDevice::SetPrimitiveTopology(CustomStruct::CRenderPrimitiveTopology::PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		}

		{
			//Shadow pass
			CGPUProfilerManager::AddPass("Shadow_Pass", m_FrameIndex, [&]() {
				CRenderDevice::SetDepthStencilState(m_ShadowPrePassDSS);
				CRenderDevice::SetBlendState(m_ShadowSkyForwardPrePassBS);
				CRenderDevice::SetRenderTarget(m_ShadowBuffer.DepthStencilView);
				for (INT i = CScene::SceneLayout::LAYOUT_TERRAIN; i < CScene::SceneLayout::LAYOUT_TRANSPARENT; ++i)
				{
					for (CGameObject* obj : m_CurrentScenePrimitives[i])
					{
						obj->DrawExtra();
					}
				}});
		}

		{
			//Pre depth pass
			CGPUProfilerManager::AddPass("Pre-Depth_Pass", m_FrameIndex, [&]() {
				CRenderDevice::SetRenderTarget(m_SceneDepth.DepthStencilView);
				for (INT i = CScene::SceneLayout::LAYOUT_TERRAIN; i < CScene::SceneLayout::LAYOUT_TRANSPARENT; ++i)
				{
					for (CGameObject* obj : m_CurrentScenePrimitives[i])
					{
						obj->DrawExtra();
					}
				}});
		}

		{
			//HZB GTAO pass
			CGPUProfilerManager::AddPass("HZB_Pass", m_FrameIndex, [&]() {
				CRenderDevice::SetNoRenderTarget();
				m_HZBPass->ComputeHZB(m_SceneDepth.ShaderResourceView); });
			CGPUProfilerManager::AddPass("GTAO_Pass", m_FrameIndex, [&]() {
				m_GTAOPass->ComputeGTAO(m_SceneDepth.ShaderResourceView); });
		}

		{
			//Occlusion culling pass
			CGPUProfilerManager::AddPass("Occlusion_Culling_Pass", m_FrameIndex, [&]() {
				m_GPUCulling->ComputeCulling(m_FrameIndex); });
		}

		{
			//Geometry buffer pass
			CGPUProfilerManager::AddPass("Geometry_Buffer_Pass", m_FrameIndex, [&]() {
				CRenderDevice::SetDepthStencilState(m_GBufferForwardPassDSS);
				CRenderDevice::SetBlendState(m_GBufferPassBS);
				Microsoft::WRL::ComPtr<ID3D11RenderTargetView> tempGBuffersRTV[CRenderPipeline::GEOMETRY_BUFFER_COUNT + 1u];
				tempGBuffersRTV[0] = m_SceneColor.RenderTargetView;
				for (UINT i = 0u; i < CRenderPipeline::GEOMETRY_BUFFER_COUNT; i++)
				{
					tempGBuffersRTV[i + 1u] = m_GBuffer[i].RenderTargetView;
				}
				CRenderDevice::SetRenderTargets(tempGBuffersRTV, CRenderPipeline::GEOMETRY_BUFFER_COUNT + 1u, m_SceneDepth.DepthStencilView);
				CRenderDevice::BindPSShaderResourceView(m_GTAOPass->GetResultShaderResourceView(), 6u);
				for (INT i = CScene::SceneLayout::LAYOUT_TERRAIN; i < CScene::SceneLayout::LAYOUT_TRANSPARENT; ++i)
				{
					for (CGameObject* obj : m_CurrentScenePrimitives[i])
					{
						if (obj->GetMeshRenderer()->GetRenderType() == CMeshRenderer::RENDER_TYPE_OPAQUE)
							obj->Draw();
					}
				}
				});
		}

		{
			//Direct light pass
			CGPUProfilerManager::AddPass("Direct_Light_Pass", m_FrameIndex, [&]() {
				CRenderDevice::SetDepthStencilState(m_DirectLightPassDSS);
				CRenderDevice::SetBlendState(m_DirectLightPassBS);
				CRenderDevice::SetRenderTarget(m_SceneColor.RenderTargetView);
				Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> tempGBuffersSRV[CRenderPipeline::GEOMETRY_BUFFER_COUNT + 1u];
				tempGBuffersSRV[0] = m_SceneColor.ShaderResourceView;
				for (UINT i = 0u; i < CRenderPipeline::GEOMETRY_BUFFER_COUNT; i++)
				{
					tempGBuffersSRV[i + 1u] = m_GBuffer[i].ShaderResourceView;
				}
				CRenderDevice::BindPSShaderResourceViews(tempGBuffersSRV, ENGINE_GBUFFER_ALL_START_SLOT, CRenderPipeline::GEOMETRY_BUFFER_COUNT + 1u);
				DrawFullScreenPolygon(CRenderPipeline::m_DirectLightShader); });
		}

		{
			//Opaque forward pass
			CGPUProfilerManager::AddPass("Opaque_Forward_Pass", m_FrameIndex, [&]() {
				CRenderDevice::SetDepthStencilState(m_GBufferForwardPassDSS);
				CRenderDevice::SetBlendState(m_ShadowSkyForwardPrePassBS);
				CRenderDevice::SetRenderTarget(m_SceneColor.RenderTargetView, m_SceneDepth.DepthStencilView);
				for (INT i = CScene::SceneLayout::LAYOUT_TERRAIN; i < CScene::SceneLayout::LAYOUT_TRANSPARENT; ++i)
				{
					for (CGameObject* obj : m_CurrentScenePrimitives[i])
					{
						if (obj->GetMeshRenderer()->GetRenderType() == CMeshRenderer::RENDER_TYPE_OPAQUE_FORWARD)
							obj->Draw();
					}
				}});
		}

		{
			//Sky box pass
			CGPUProfilerManager::AddPass("Sky_Box_Pass", m_FrameIndex, [&]() {
				m_SkyBox->Draw(); });
		}

		{
			//Transparent forward pass
			CGPUProfilerManager::AddPass("Transparent_Forward_Pass", m_FrameIndex, [&]() {
				CRenderDevice::SetBlendState(m_TransparentPassBS);
				for (CGameObject* obj : m_CurrentScenePrimitives[CScene::SceneLayout::LAYOUT_TRANSPARENT])
				{
					if (obj->GetMeshRenderer()->GetRenderType() == CMeshRenderer::RENDER_TYPE_TRANSPARENT_FORWARD)
						obj->Draw();
				}});
		}

		{
			//TODO Test and debug.
			CGPUProfilerManager::AddPass("Debug_Pass", m_FrameIndex, [&]() {
				CRenderDevice::SetDepthStencilState(m_GBufferForwardPassDSS);
				CRenderDevice::SetBlendState(m_ShadowSkyForwardPrePassBS);
				CRenderDevice::SetRenderTarget(m_SceneColor.RenderTargetView);
				m_DebugScreen->Draw();
				m_HZBPass->DrawDebug();
				m_GTAOPass->DrawDebug(); });
		}

		{
			//Post process pass
		}
		
		{
			ImGui::Begin("GPU Profiler");
			ImGui::Text("Frame Rate Average %.3f ms/frame (%.1f FPS)", 1000.f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::Text("Shadow Pass          : %f ms.", CGPUProfilerManager::GetPassAverageTime("Shadow_Pass") * static_cast<DOUBLE>(1000));
			ImGui::Text("Pre-Depth Pass       : %f ms.", CGPUProfilerManager::GetPassAverageTime("Pre-Depth_Pass") * static_cast<DOUBLE>(1000));
			ImGui::Text("Build HZB            : %f ms.", CGPUProfilerManager::GetPassAverageTime("HZB_Pass") * static_cast<DOUBLE>(1000));
			ImGui::Text("Ambient Occlusion    : %f ms.", CGPUProfilerManager::GetPassAverageTime("GTAO_Pass") * static_cast<DOUBLE>(1000));
			ImGui::Text("Occlusion Culling    : %f ms.", CGPUProfilerManager::GetPassAverageTime("Occlusion_Culling_Pass") * static_cast<DOUBLE>(1000));
			ImGui::Text("Geometry Pass        : %f ms.", CGPUProfilerManager::GetPassAverageTime("Geometry_Buffer_Pass") * static_cast<DOUBLE>(1000));
			ImGui::Text("Sky Pass             : %f ms.", CGPUProfilerManager::GetPassAverageTime("Sky_Box_Pass") * static_cast<DOUBLE>(1000));
			ImGui::Text("Direct Light Pass    : %f ms.", CGPUProfilerManager::GetPassAverageTime("Direct_Light_Pass") * static_cast<DOUBLE>(1000));
			ImGui::Text("Opaque Forward Pass  : %f ms.", CGPUProfilerManager::GetPassAverageTime("Opaque_Forward_Pass") * static_cast<DOUBLE>(1000));
			ImGui::Text("Transparent Pass     : %f ms.", CGPUProfilerManager::GetPassAverageTime("Transparent_Forward_Pass") * static_cast<DOUBLE>(1000));
			ImGui::Text("Debug Screen Info    : %f ms.", CGPUProfilerManager::GetPassAverageTime("Debug_Pass") * static_cast<DOUBLE>(1000));
			ImGui::End();
		}

		{
			//Final output
			CRenderDevice::SetDefaultDepthStencilState();
			CRenderDevice::SetDefaultBlendState();
			CRenderDevice::SetFinalOutput();
			CRenderDevice::BindPSShaderResourceView(m_SceneColor.ShaderResourceView, ENGINE_TEXTURE2D_ALBEDO_START_SLOT);
			DrawFullScreenPolygon(CRenderPipeline::m_ScreenPolygonShader);
		}
	}
}
void CRenderPipeline::PreparePerFrameRender(CCamera* camera)
{
	CustomType::Vector2 cameraViewportMin = camera->GetViewportMinSize();
	CustomStruct::CRenderViewport vp = camera->GetViewport();

	m_RenderPerFrameInfo.PerFrameData.ProjectionMatrix = camera->GetProjectionMatrix().GetGPUUploadFloat4x4();
	m_RenderPerFrameInfo.PerFrameData.ProjectionInvMatrix = camera->GetProjectionInverseMatrix().GetGPUUploadFloat4x4();
	m_RenderPerFrameInfo.PerFrameData.DepthMultiAdd = camera->GetDeviceZToViewZMulAdd().GetXMFLOAT4();
	m_RenderPerFrameInfo.PerFrameData.ScreenToViewSpaceParams = camera->GetScreenToViewParameters(CustomType::Vector2Int(CRenderDevice::GetViewport().Width, CRenderDevice::GetViewport().Height), CustomType::Vector2Int(CRenderDevice::GetViewport().Width, CRenderDevice::GetViewport().Height)).GetXMFLOAT4();
	m_RenderPerFrameInfo.PerFrameData.CameraViewportMinSizeAndInvBufferSize = DirectX::XMFLOAT4(cameraViewportMin.X(), cameraViewportMin.Y(), 1.f / static_cast<FLOAT>(m_GlobalBufferSize.X()), 1.f / static_cast<FLOAT>(m_GlobalBufferSize.Y()));
	m_RenderPerFrameInfo.PerFrameData.CameraViewportSizeAndInvSize = camera->GetViewportSizeAndInvSize().GetXMFLOAT4();
	m_RenderPerFrameInfo.PerFrameData.CameraViewportRect = DirectX::XMFLOAT4(vp.TopLeftX, vp.TopLeftY, vp.Width, vp.Height);

	m_RenderPerFrameInfo.PerFrameData.ViewMatrix = camera->GetViewMatrix().GetGPUUploadFloat4x4();
	m_RenderPerFrameInfo.PerFrameData.ViewInvMatrix = camera->GetViewInverseMatrix().GetGPUUploadFloat4x4();
	m_RenderPerFrameInfo.PerFrameData.ViewProjectionMatrix = camera->GetViewProjectionMatrix().GetGPUUploadFloat4x4();
	m_RenderPerFrameInfo.PerFrameData.ViewProjectionInvMatrix = camera->GetViewProjectionInverseMatrix().GetGPUUploadFloat4x4();
	m_RenderPerFrameInfo.PerFrameData.TimeParams = DirectX::XMFLOAT4(static_cast<FLOAT>(CManager::GetGameTimer()->GetClockTime()), static_cast<FLOAT>(CManager::GetGameTimer()->GetDeltaTime()), 1.f, 1.f);
	m_RenderPerFrameInfo.PerFrameData.CameraWorldPosition = camera->GetPosition().GetXMFLOAT4();
}
void CRenderPipeline::PrepareCameraCullingInfo(CRenderCameraCullingInfo& cullingInfo, CCamera* camera)
{
	std::vector<CustomType::Vector3> cameraPlane(camera->GetCullingPlane());
	cullingInfo.OriginPosition = camera->GetPosition();
	cullingInfo.ClipPlane[0u] = camera->GetForwardVector();
	cullingInfo.ClipPlane[1u] = -cameraPlane[0u];
	cullingInfo.ClipPlane[2u] = -cameraPlane[1u];
	cullingInfo.ClipPlane[3u] = -cameraPlane[2u];
	cullingInfo.ClipPlane[4u] = -cameraPlane[3u];
	for (INT i = 0; i < 5u; i++)
	{
		cullingInfo.ClipDot[i] = CustomType::Vector3::Dot(cullingInfo.ClipPlane[i], cullingInfo.OriginPosition) + cullingInfo.ClipOffset;
	}
}
BOOL CRenderPipeline::CullingCameraPlane(const CustomType::Vector3& pos, const FLOAT& radius, const CRenderCameraCullingInfo& cullingInfo)
{
	for (UINT i = 0u; i < 5u; i++)
	{
		if ((CustomType::Vector3::Dot(pos, cullingInfo.ClipPlane[i]) + radius) < cullingInfo.ClipDot[i])
		{
			return FALSE;
		}
	}
	return TRUE;
}
void CRenderPipeline::Culling(std::vector<CGameObject*>& cullingResult, const CRenderCameraCullingInfo& cullingInfo, const std::vector<CGameObject*>& primitives)
{
	for (UINT i = 0u; i < primitives.size(); i++)
	{
		CustomType::Vector3 objPos; FLOAT objRadius;
		primitives[i]->GetBoundingSphere(objPos, objRadius);
		if ((CustomType::Vector3::Distance(objPos, cullingInfo.OriginPosition) - objRadius) < cullingInfo.Distance)
		{
			if (CullingCameraPlane(objPos, objRadius, cullingInfo))
			{
				cullingResult.push_back(primitives[i]);
			}
		}
	}
}
void CRenderPipeline::PrepareLightDataRender()
{
	std::vector<CLightDirectional*> lightDirectional;
	std::vector<CLightPoint*> lightPoint;
	std::vector<CLightSpot*> lightSpot;
	for (CGameObject* obj : m_CurrentScenePrimitives[CScene::SceneLayout::LAYOUT_LIGHT])
	{
		if (!obj)
		{
			continue;
		}
		CLightBase* lightObj = reinterpret_cast<CLightBase*>(obj);
		if (!lightObj)
		{
			continue;
		}
		CLightBase::LightType lightType = lightObj->GetLightType();
		if (lightType == CLightBase::LightType::LIGHT_TYPE_DIRECTIONAL)
		{
			CLightDirectional* tempLight = reinterpret_cast<CLightDirectional*>(lightObj);
			if (tempLight)
			{
				lightDirectional.push_back(tempLight);
			}
		}
		else if (lightType == CLightBase::LightType::LIGHT_TYPE_POINT)
		{
			CLightPoint* tempLight = reinterpret_cast<CLightPoint*>(lightObj);
			if (tempLight)
			{
				lightPoint.push_back(tempLight);
			}
		}
		else if (lightType == CLightBase::LightType::LIGHT_TYPE_SPOT)
		{
			CLightSpot* tempLight = reinterpret_cast<CLightSpot*>(lightObj);
			if (tempLight)
			{
				lightSpot.push_back(tempLight);
			}
		}
	}
	m_RenderLightDataInfo.LightData.LightCount = DirectX::XMINT4(static_cast<INT>(lightDirectional.size()), static_cast<INT>(lightPoint.size()), static_cast<INT>(lightSpot.size()), 0);
	UINT lightIndex = 0u;
	for (INT i = 0; i < m_RenderLightDataInfo.LightData.LightCount.x; i++)
	{
		if (lightIndex >= CustomStruct::CShaderGlobalLightData::GetSupportLightMaxCount())
		{
			break;
		}

		CustomStruct::CColor color; FLOAT intensity;
		lightDirectional[i]->GetColor(color, intensity);
		CustomType::Vector3 direction(lightDirectional[i]->GetForwardVector());

		m_RenderLightDataInfo.LightData.LightParams[lightIndex].Color = DirectX::XMFLOAT4(color.r, color.g, color.b, intensity);
		m_RenderLightDataInfo.LightData.LightParams[lightIndex].Params0 = DirectX::XMFLOAT4(direction.X(), direction.Y(), direction.Z(), 0.f);
		m_RenderLightDataInfo.LightData.LightParams[lightIndex].Params1 = DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.f);

		lightIndex += 1u;
	}
	for (INT i = 0; i < m_RenderLightDataInfo.LightData.LightCount.y; i++)
	{
		if (lightIndex >= CustomStruct::CShaderGlobalLightData::GetSupportLightMaxCount())
		{
			break;
		}

		CustomStruct::CColor color; FLOAT intensity;
		lightPoint[i]->GetColor(color, intensity);
		CustomType::Vector3 position(lightPoint[i]->GetPosition());

		m_RenderLightDataInfo.LightData.LightParams[lightIndex].Color = DirectX::XMFLOAT4(color.r, color.g, color.b, intensity);
		m_RenderLightDataInfo.LightData.LightParams[lightIndex].Params0 = DirectX::XMFLOAT4(position.X(), position.Y(), position.Z(), lightPoint[i]->GetRadius());
		m_RenderLightDataInfo.LightData.LightParams[lightIndex].Params1 = DirectX::XMFLOAT4(0.f, 0.f, 0.f, lightPoint[i]->GetAttenuationExponent());

		lightIndex += 1u;
	}
	for (INT i = 0; i < m_RenderLightDataInfo.LightData.LightCount.z; i++)
	{
		if (lightIndex >= CustomStruct::CShaderGlobalLightData::GetSupportLightMaxCount())
		{
			break;
		}

		CustomStruct::CColor color; FLOAT intensity;
		lightSpot[i]->GetColor(color, intensity);
		CustomType::Vector3 position(lightSpot[i]->GetPosition());
		CustomType::Vector3 direction(lightSpot[i]->GetForwardVector());

		m_RenderLightDataInfo.LightData.LightParams[lightIndex].Color = DirectX::XMFLOAT4(color.r, color.g, color.b, intensity);
		m_RenderLightDataInfo.LightData.LightParams[lightIndex].Params0 = DirectX::XMFLOAT4(position.X(), position.Y(), position.Z(), lightSpot[i]->GetRange());
		m_RenderLightDataInfo.LightData.LightParams[lightIndex].Params1 = DirectX::XMFLOAT4(direction.X(), direction.Y(), direction.Z(), lightSpot[i]->GetCosHalfRadian());

		lightIndex += 1u;
	}
}
void CRenderPipeline::DrawFullScreenPolygon(const std::shared_ptr<CPixelShader>& shader)
{
	m_FullScreenPolygonVS->Bind();
	shader->Bind();
	CRenderDevice::SetVertexBuffer(m_FullScreenPolygon->GetVertexBuffer(), m_FullScreenPolygon->GetVertexStride());
	CRenderDevice::SetIndexBuffer(m_FullScreenPolygon->GetIndexBuffer());
	CRenderDevice::DrawIndexed(m_FullScreenPolygon->GetIndexCount());
}
CTexture2D* CRenderPipeline::GetDefaultTexture(CustomStruct::CEngineDefaultTexture2DEnum input)
{
	return (CRenderPipeline::m_DefaultTexture[input]);
}
std::shared_ptr<CPixelShader> CRenderPipeline::GetDefaultEmptyPS()
{
	return (CRenderPipeline::m_DefaultEmptyPS);
}