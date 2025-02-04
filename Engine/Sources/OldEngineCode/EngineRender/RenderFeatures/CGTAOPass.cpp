/*
#include "../Headers/CGTAOPass.h"
#include "../../RenderBase/Headers/CRenderPipeline.h"
#include "../../RenderBase/Headers/CMeshRendererComponent.h"
#include "../../AssetsManager/Headers/CTextureType.h"
#include "../../../EngineGame/Headers/CCamera.h"
#include "../../../EngineGame/Headers/CScene.h"

#ifdef _DEVELOPMENT_EDITOR
#include "../../../EngineGame/Headers/CScreenPolygon2D.h"
#endif

CGTAOPass::CGTAOPass()
{
	this->m_MainCamera = NULL;
	this->m_BufferSize = CustomType::Vector2Int(0, 0);
	this->m_PipelineSize = CustomType::Vector2Int(0, 0);

	this->m_UserArguments.NumAngles = 2u;
	this->m_UserArguments.FallOffEnd = 200.f;
	//this->m_UserArguments.FallOffStartRatio = 0.5f;
	this->m_UserArguments.ThicknessBlend = 0.5f;
	this->m_UserArguments.FadeRadius = 5000.f;
	this->m_UserArguments.FadeDistance = 8000.f;
	this->m_UserArguments.Intensity = 0.5f;
	this->m_UserArguments.Power = 2.f;

#ifdef _DEVELOPMENT_EDITOR
	this->m_Polygon2D = NULL;
	this->m_DebugType = 0;
#endif
}
CGTAOPass::~CGTAOPass()
{
#ifdef _DEVELOPMENT_EDITOR
	if (this->m_Polygon2D != NULL)
	{
		delete (this->m_Polygon2D);
		this->m_Polygon2D = NULL;
	}
#endif
}
void CGTAOPass::Init(CCamera* mainCamera, const CustomType::Vector2Int& pipelineSize)
{
	this->m_MainCamera = mainCamera;
	this->m_PipelineSize = pipelineSize;

	INT32 widthBuffer = (pipelineSize.X() + 1) / 2;
	INT32 heightBuffer = (pipelineSize.Y() + 1) / 2;

	this->m_BufferSize = CustomType::Vector2Int(widthBuffer, heightBuffer);

	UINT32 bufferWidth = static_cast<UINT32>(widthBuffer);
	UINT32 bufferHeight = static_cast<UINT32>(heightBuffer);

	CRenderDevice::CreateBuffer(this->m_ConstantBuffer,
		CustomStruct::CRenderBufferDesc(
			sizeof(GTAOConstantBuffer),
			CustomStruct::CRenderBindFlag::BIND_CONSTANT_BUFFER,
			sizeof(FLOAT)));
	CRenderDevice::CreateRenderTexture2D(
		this->m_IntegralBuffer,
		CustomStruct::CRenderTextureDesc(
			bufferWidth,
			bufferHeight,
			CustomStruct::CRenderBindFlag::BIND_SRV_UAV,
			CustomStruct::CRenderFormat::FORMAT_R8G8B8A8_UNORM));
	CRenderDevice::CreateRenderTexture2D(
		this->m_FilterBuffer,
		CustomStruct::CRenderTextureDesc(
			bufferWidth,
			bufferHeight,
			CustomStruct::CRenderBindFlag::BIND_SRV_UAV,
			CustomStruct::CRenderFormat::FORMAT_R8_UNORM));

	CRenderDevice::LoadComputeShader("./Engine/Assets/EngineShaders/GTAOSpatialIntegral.cso", this->m_IntegralComputeShader);
	CRenderDevice::LoadComputeShader("./Engine/Assets/EngineShaders/GTAOSpatialFilter.cso", this->m_FilterComputeShader);

#ifdef _DEVELOPMENT_EDITOR
	this->m_Polygon2D = new CScreenPolygon2D(TRUE, nullptr, ENGINE_SHADER_SCREEN_POLYGON_2D_PS, CustomType::Vector4(0, 0, pipelineSize.X(), pipelineSize.Y()));
	this->m_Polygon2D->Init();
	CRenderDevice::LoadComputeShader("./Engine/Assets/EngineShaders/GTAODebugType.cso", this->m_DebugComputeShader);
	CRenderDevice::CreateRenderTexture2D(
		this->m_DebugBuffer,
		CustomStruct::CRenderTextureDesc(
			bufferWidth,
			bufferHeight,
			CustomStruct::CRenderBindFlag::BIND_SRV_UAV,
			CustomStruct::CRenderFormat::FORMAT_R8G8B8A8_UNORM));
#endif
}
void CGTAOPass::Uninit()
{
#ifdef _DEVELOPMENT_EDITOR
	if (this->m_Polygon2D != NULL)
	{
		delete (this->m_Polygon2D);
		this->m_Polygon2D = NULL;
	}
#endif
}
void CGTAOPass::Update()
{
	FLOAT bufferSizeW = static_cast<FLOAT>(this->m_BufferSize.X());
	FLOAT bufferSizeH = static_cast<FLOAT>(this->m_BufferSize.Y());
	FLOAT pipelineSizeW = static_cast<FLOAT>(this->m_PipelineSize.X());
	FLOAT pipelineSizeH = static_cast<FLOAT>(this->m_PipelineSize.Y());

	FLOAT fallOffEnd = this->m_UserArguments.FallOffEnd;
	//FLOAT fallOffStartRatio = this->m_UserArguments.FallOffStartRatio;
	//FLOAT fallOffStart = fallOffEnd * fallOffStartRatio;
	//FLOAT fallOffStartSq = fallOffStart * fallOffStart;
	//FLOAT fallOffEndSq = fallOffEnd * fallOffEnd;

	//FLOAT fallOffScale = 1.f / (fallOffEndSq - fallOffStartSq);
	//FLOAT fallOffBias = -fallOffStartSq * fallOffScale;

	FLOAT thicknessBlend = this->m_UserArguments.ThicknessBlend;
	thicknessBlend = CustomType::CMath::Clamp(1.f - (thicknessBlend * thicknessBlend), 0.f, 0.99f);

	FLOAT numAngles = static_cast<FLOAT>(this->m_UserArguments.NumAngles);
	FLOAT sinDeltaAngle, cosDeltaAngle;
	CustomType::CMath::SinCos(sinDeltaAngle, cosDeltaAngle, CustomType::CMath::GetPI() / numAngles);

	this->m_ConstantData.AdjustAngleThicknessParams = DirectX::XMFLOAT4(fallOffEnd * pipelineSizeH * this->m_MainCamera->GetProjectionMatrix().GetXMFLOAT4X4()._11, sinDeltaAngle, cosDeltaAngle, thicknessBlend);

	FLOAT fadeRadius = CustomType::CMath::Max(1.f, this->m_UserArguments.FadeRadius);
	FLOAT invFadeRadius = 1.f / fadeRadius;

	this->m_ConstantData.FadeAttenParams = DirectX::XMFLOAT4(invFadeRadius, -(this->m_UserArguments.FadeDistance - fadeRadius) * invFadeRadius, this->m_UserArguments.FadeDistance, 2.f / (fallOffEnd * fallOffEnd));

	this->m_ConstantData.ResultBufferParams = DirectX::XMFLOAT4(bufferSizeW, bufferSizeH, 1.f / bufferSizeW, 1.f / bufferSizeH);
	this->m_ConstantData.DepthBufferParams = DirectX::XMFLOAT4(pipelineSizeW, pipelineSizeH, 1.f / pipelineSizeW, 1.f / pipelineSizeH);
	this->m_ConstantData.IntensityPowerParams = DirectX::XMFLOAT4(this->m_UserArguments.Intensity, this->m_UserArguments.Power * 0.5f, static_cast<FLOAT>(this->m_DebugType), 0.f);
}
void CGTAOPass::ComputeGTAO(const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& sceneDepth)
{
	UINT32 dispatchX = static_cast<UINT32>((this->m_BufferSize.X() + 7) / 8);
	UINT32 dispatchY = static_cast<UINT32>((this->m_BufferSize.Y() + 7) / 8);
	UINT32 dispatchZ = 1u;

	CRenderDevice::UploadBuffer(this->m_ConstantBuffer, &(this->m_ConstantData));
	CRenderDevice::BindCSConstantBuffer(this->m_ConstantBuffer, 3u);

	CRenderDevice::BindCSShaderResourceView(sceneDepth, 5u);
	CRenderDevice::BindCSUnorderedAccessView(this->m_IntegralBuffer.UnorderedAccessView, 0u);
	CRenderDevice::SetCSShader(this->m_IntegralComputeShader);
	CRenderDevice::Dispatch(dispatchX, dispatchY, dispatchZ);
	CRenderDevice::BindNoCSUnorderedAccessView(0u);
	CRenderDevice::SetNoCSShader();

	CRenderDevice::BindCSShaderResourceView(this->m_IntegralBuffer.ShaderResourceView, 5u);
	CRenderDevice::BindCSUnorderedAccessView(this->m_FilterBuffer.UnorderedAccessView, 0u);
	CRenderDevice::SetCSShader(this->m_FilterComputeShader);
	CRenderDevice::Dispatch(dispatchX, dispatchY, dispatchZ);
	CRenderDevice::BindNoCSUnorderedAccessView(0u);
	CRenderDevice::SetNoCSShader();
}
#ifdef _DEVELOPMENT_EDITOR
void CGTAOPass::EditorUpdate()
{
	INT32 tempNumAngles = this->m_UserArguments.NumAngles;

	ImGui::Begin("GTAO parameters");
	ImGui::SliderInt("Debug type", &(this->m_DebugType), 0, 3);
	ImGui::SliderInt("Angles number", &(tempNumAngles), 1, 16);
	ImGui::SliderFloat("Fall off end", &(this->m_UserArguments.FallOffEnd), 0.f, 1000.f);
	//ImGui::SliderFloat("Fall off start ratio", &(this->m_UserArguments.FallOffStartRatio), 0.f, 0.999f);
	ImGui::SliderFloat("Thickness blend", &(this->m_UserArguments.ThicknessBlend), 0.f, 1.f);
	ImGui::SliderFloat("Fade radius", &(this->m_UserArguments.FadeRadius), 0.f, 200000.f);
	ImGui::SliderFloat("Fade distance", &(this->m_UserArguments.FadeDistance), 0.f, 200000.f);
	ImGui::SliderFloat("Intensity", &(this->m_UserArguments.Intensity), 0.f, 1.f);
	ImGui::SliderFloat("Power", &(this->m_UserArguments.Power), 0.1f, 8.f);
	ImGui::End();

	this->m_UserArguments.NumAngles = tempNumAngles;
}
void CGTAOPass::DrawDebug()
{
	if (this->m_DebugType != 0)
	{
		{
			UINT32 dispatchX = static_cast<UINT32>((this->m_BufferSize.X() + 7) / 8);
			UINT32 dispatchY = static_cast<UINT32>((this->m_BufferSize.Y() + 7) / 8);
			UINT32 dispatchZ = 1u;
			CRenderDevice::BindCSConstantBuffer(this->m_ConstantBuffer, 3u);
			CRenderDevice::BindCSShaderResourceView(this->m_IntegralBuffer.ShaderResourceView, 5u);
			CRenderDevice::BindCSShaderResourceView(this->m_FilterBuffer.ShaderResourceView, 6u);
			CRenderDevice::BindCSUnorderedAccessView(this->m_DebugBuffer.UnorderedAccessView, 0u);
			CRenderDevice::SetCSShader(this->m_DebugComputeShader);
			CRenderDevice::Dispatch(dispatchX, dispatchY, dispatchZ);
			CRenderDevice::BindNoCSUnorderedAccessView(0u);
			CRenderDevice::SetNoCSShader();
		}

		CRenderDevice::BindPSShaderResourceView(this->m_DebugBuffer.ShaderResourceView, ENGINE_GBUFFER_A_START_SLOT);
		{
			const CMeshRendererComponent* meshRenderer = this->m_Polygon2D->GetMeshRendererComponent<CMeshRendererComponent>();
			if (meshRenderer)
			{
				meshRenderer->Draw();
			}
		}
	}
}
#endif
*/