#include "../Headers/CDebugScreen.h"
#include "../../EngineBase/Headers/CManager.h"
#include "../../EngineRender/RenderBase/Headers/CRenderDevice.h"
#include "../../EngineRender/RenderBase/Headers/CRenderPipeline.h"
#include "../../EngineGame/Headers/CCamera.h"
#include "../../EngineGame/Headers/CScene.h"
#include "../../EngineGame/Headers/CScreenPolygon2D.h"

UINT CDebugScreen::DEBUGPOLYGON_COUNT = 0u;
CDebugScreen::CDebugScreen()
{
	if (CDebugScreen::DEBUGPOLYGON_COUNT == 0u)
	{
		CDebugScreen::DEBUGPOLYGON_COUNT = (CManager::GetRenderPipeline()->GEOMETRY_BUFFER_COUNT + 1u + 2u);
	}
}
CDebugScreen::~CDebugScreen()
{
}
void CDebugScreen::Init(const CustomType::Vector2Int& pipelineSize)
{
	INT length = pipelineSize.Y() / CDebugScreen::DEBUGPOLYGON_COUNT;
	INT polygonCount = static_cast<INT>(CDebugScreen::DEBUGPOLYGON_COUNT);
	for (INT i = 0; i < polygonCount; i++)
	{
		CustomType::Vector4 tempScreenAnchor = CustomType::Vector4(0, length * i, length, length * (i + 1));
		CScreenPolygon2D* tempScreenPolygon2D = new CScreenPolygon2D(ENGINE_SHADER_SCREEN_POLYGON_2D_VS, ENGINE_SHADER_SCREEN_POLYGON_2D_PS, tempScreenAnchor);
		tempScreenPolygon2D->Init();
		m_Polygons.push_back(tempScreenPolygon2D);
	}
	m_SRVs.clear();
}
void CDebugScreen::Uninit()
{
	UINT size = static_cast<UINT>(m_Polygons.size());
	if (size > 0u)
	{
		for (UINT i = 0u; i < size; i++)
		{
			if (m_Polygons[i] != NULL)
				delete (m_Polygons[i]);
		}
	}
	m_Polygons.clear();
	m_SRVs.clear();
}
void CDebugScreen::Update()
{
}
void CDebugScreen::PrepareDraw()
{
	if (m_SRVs.size() != CDebugScreen::DEBUGPOLYGON_COUNT)
	{
		m_SRVs.resize(CDebugScreen::DEBUGPOLYGON_COUNT);
		m_SRVs[0] = CManager::GetRenderPipeline()->m_SceneColor.ShaderResourceView;
		for (INT i = 0; i < static_cast<INT>(CManager::GetRenderPipeline()->GEOMETRY_BUFFER_COUNT); i++)
		{
			m_SRVs[i + 1] = CManager::GetRenderPipeline()->m_GBuffer[i].ShaderResourceView;
		}
		m_SRVs[CManager::GetRenderPipeline()->GEOMETRY_BUFFER_COUNT + 1] = CManager::GetRenderPipeline()->m_SceneDepth.ShaderResourceView;
		m_SRVs[CManager::GetRenderPipeline()->GEOMETRY_BUFFER_COUNT + 2] = CManager::GetRenderPipeline()->m_ShadowBuffer.ShaderResourceView;
	}
}
void CDebugScreen::Draw()
{
	this->PrepareDraw();
	for (UINT i = 0; i < CDebugScreen::DEBUGPOLYGON_COUNT; i++)
	{
		CRenderDevice::BindPSShaderResourceView(m_SRVs[i], ENGINE_TEXTURE2D_ALBEDO_START_SLOT);
		m_Polygons[i]->Draw();
	}
}