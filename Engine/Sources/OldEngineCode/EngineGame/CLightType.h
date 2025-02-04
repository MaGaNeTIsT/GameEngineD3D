/*
#pragma once

#include "../../../../Entry/EngineMain.h"
#include "../../EngineRender/RenderBase/Headers/CRenderDevice.h"
#include "./CGameObject.h"

class CLightBase : public CGameObject
{
	EClass(CLightBase, CGameObject)
public:
	struct LightShadowInfo
	{
		LightShadowInfo() { ::ZeroMemory(this, sizeof(*this)); }
		CustomType::Vector2Int					Size;
		UINT32									Depth;
		CRenderDevice::RenderTexture2DViewInfo	Texture;
	};
	enum LightType
	{
		LIGHT_TYPE_NONE			= 0,
		LIGHT_TYPE_DIRECTIONAL	= 1,
		LIGHT_TYPE_POINT		= 2,
		LIGHT_TYPE_SPOT			= 3
	};
public:
	virtual BOOL32	IsTransmitShadow() = 0;
	LightType		GetLightType() { return (this->m_LightType); }
	void			GetColor(CustomStruct::CColor& color, FLOAT& intensity);
	void			SetColor(const CustomStruct::CColor& color, const FLOAT& intensity);
public:
	virtual CustomType::Matrix4x4	GetCurrentViewMatrix(const UINT32& extraIndex = 0u) = 0;
	virtual CustomType::Matrix4x4	GetCurrentProjectionMatrix(const UINT32& extraIndex = 0u) = 0;
	virtual CustomType::Matrix4x4	GetPreviousViewMatrix(const UINT32& extraIndex = 0u) = 0;
	virtual CustomType::Matrix4x4	GetPreviousProjectionMatrix(const UINT32& extraIndex = 0u) = 0;
protected:
	static BOOL32		CreateShadowTexture(CRenderDevice::RenderTexture2DViewInfo& output, const CustomType::Vector2Int& shadowSize, const UINT32& shadowDepth);
	void			SetLightType(LightType type) { this->m_LightType = type; }
public:
	CLightBase(const BOOL32& active, const class CScene* scene);
	virtual ~CLightBase() {}
protected:
	LightType					m_LightType;
	CustomStruct::CColor		m_Color;
	FLOAT						m_Intensity;
};

class CLightDirectional : public CLightBase
{
	EClass(CLightDirectional, CLightBase)
public:
	struct ShadowCascadeSettings
	{
		ShadowCascadeSettings()
		{
			::ZeroMemory(this, sizeof(*this));
			const FLOAT defaultDistance = ENGINE_DEFAULT_CULLING_DISTANCE;
			LayerNum = 1u;
			Distance[1u] = defaultDistance;
		}
		ShadowCascadeSettings(const UINT32& layerNum, const std::vector<FLOAT>& distances, const std::vector<FLOAT>& borders, const BOOL32& isBorderPercentage = TRUE)
		{
			::ZeroMemory(this, sizeof(*this));
			const FLOAT defaultDistance = ENGINE_DEFAULT_CULLING_DISTANCE;
			const FLOAT defaultPercentage = 0.1f;	// default 10% border of full distance.
			LayerNum = CustomType::CMath::Clamp(layerNum, 1u, 4u);
			for (UINT32 i = 0u; i < LayerNum; i++)
			{
				if (i < distances.size())
				{
					Distance[i] = distances[i];
				}
			}
			for (UINT32 i = 0u; i < (LayerNum - 1); i++)
			{
				if (i < borders.size() && i < distances.size())
				{
					if (isBorderPercentage)
					{
						Border[i] = borders[i];
					}
					else
					{
						Border[i] = CustomType::CMath::Clamp(borders[i] / distances[i], 0.f, 1.f);
					}
				}
			}
		}
		UINT32	LayerNum;
		FLOAT	Distance[4];
		FLOAT	Border[3];
	};
public:
	virtual BOOL32	IsTransmitShadow()override;
	virtual BOOL32	SetShadowInfo(const CustomType::Vector2Int& shadowSize, const UINT32& shadowDepth);
	void			PrepareCascadeShadowInfo(class CCamera* camera, const ShadowCascadeSettings* settings = NULL);
	void			UpdateCascadeShadowInfo();
public:
	virtual const UINT32&						GetCurrentShadowMapLayerNum();
	virtual CustomStruct::CRenderViewport	GetCurrentShadowMapViewport(const UINT32& extraIndex);
	virtual const LightShadowInfo&			GetCurrentShadowMap(const UINT32& extraIndex);
	virtual CustomType::Vector4				GetCurrentProjectionSphereBounding(const UINT32& extraIndex);
	virtual CustomType::Matrix4x4			GetCurrentViewMatrix(const UINT32& extraIndex = 0u)override;
	virtual CustomType::Matrix4x4			GetCurrentProjectionMatrix(const UINT32& extraIndex = 0u)override;
	virtual CustomType::Matrix4x4			GetPreviousViewMatrix(const UINT32& extraIndex = 0u)override;
	virtual CustomType::Matrix4x4			GetPreviousProjectionMatrix(const UINT32& extraIndex = 0u)override;
public:
	virtual void	Init()override;
	virtual void	Update()override;
#ifdef _DEVELOPMENT_EDITOR
public:
	virtual void	SelectedEditorUpdate()override;
#endif
protected:
	static BOOL32		GenerateClosestShadowMap(CLightDirectional* light);
	static BOOL32		GenerateCascadeShadowMap(CLightDirectional* light, const INT32& frameCounter);
	static void		GenerateCascadeMatrices(class CCamera* camera, CLightDirectional* light);
public:
	CLightDirectional(const BOOL32& active, const class CScene* scene);
	virtual ~CLightDirectional() {}
protected:
	struct ShadowCascadeLayerInfo
	{
		ShadowCascadeLayerInfo() { ::ZeroMemory(this, sizeof(*this)); }
		UINT32				LayerNum;
		std::vector<FLOAT>	Distances;
		std::vector<FLOAT>	Borders;
	};
	struct ShadowCascadeInfo
	{
		ShadowCascadeInfo() { ::ZeroMemory(this, sizeof(*this)); }
		class CCamera*							CurrentCamera;
		ShadowCascadeSettings					CascadeSettings;
		ShadowCascadeLayerInfo					LayerInfo;
		CustomType::Matrix4x4					ViewMatrix;
		std::vector<CustomType::Matrix4x4>		ProjectionMatrices;
		std::vector<CustomType::Vector4>		ProjectionSphereBounds;
		CustomStruct::CRenderViewport			Viewport;
		std::vector<LightShadowInfo>			ShadowMap;
	};
protected:
	INT32									m_FrameCounter;
	BOOL32								m_IsTransmitShadow;
	std::shared_ptr<ShadowCascadeInfo>	m_CascadeInfo[2];
};

class CLightPoint : public CLightBase
{
public:
	virtual BOOL32	IsTransmitShadow()override { return FALSE; }
	const FLOAT&	GetRadius()const { return (this->m_Radius); }
	void			SetRadius(const FLOAT& value) { this->m_Radius = value; }
	const FLOAT&	GetAttenuationExponent()const { return (this->m_AttenuationExponent); }
	void			SetAttenuationExponent(const FLOAT& value) { this->m_AttenuationExponent = value; }
public:
	virtual CustomType::Matrix4x4		GetCurrentViewMatrix(const UINT32& extraIndex = 0u)override;
	virtual CustomType::Matrix4x4		GetCurrentProjectionMatrix(const UINT32& extraIndex = 0u)override;
	virtual CustomType::Matrix4x4		GetPreviousViewMatrix(const UINT32& extraIndex = 0u)override;
	virtual CustomType::Matrix4x4		GetPreviousProjectionMatrix(const UINT32& extraIndex = 0u)override;
public:
	virtual void	Init()override;
public:
	CLightPoint(const BOOL32& active, const class CScene* scene);
	virtual ~CLightPoint() {}
protected:
	FLOAT	m_AttenuationExponent;
	FLOAT	m_Radius;
};

class CLightSpot : public CLightBase
{
public:
	virtual BOOL32	IsTransmitShadow()override { return FALSE; }
	const FLOAT&	GetRange()const { return (this->m_Range); }
	void			SetRange(const FLOAT& v) { this->m_Range = v; }
	const FLOAT&	GetHalfRadian()const { return (this->m_HalfRadian); }
	void			SetHalfRadian(const FLOAT& v) { this->m_HalfRadian = v; }
	const FLOAT&	GetCosHalfRadian()const { return (this->m_CosHalfRadian); }
	void			SetCosHalfRadian(const FLOAT& v) { this->m_CosHalfRadian = v; }
public:
	virtual CustomType::Matrix4x4		GetCurrentViewMatrix(const UINT32& extraIndex = 0u)override;
	virtual CustomType::Matrix4x4		GetCurrentProjectionMatrix(const UINT32& extraIndex = 0u)override;
	virtual CustomType::Matrix4x4		GetPreviousViewMatrix(const UINT32& extraIndex = 0u)override;
	virtual CustomType::Matrix4x4		GetPreviousProjectionMatrix(const UINT32& extraIndex = 0u)override;
public:
	virtual void	Init()override;
	virtual void	Update()override;
public:
	CLightSpot(const BOOL32& active, const class CScene* scene);
	virtual ~CLightSpot() {}
protected:
	FLOAT					m_Range;
	FLOAT					m_HalfRadian;
	FLOAT					m_CosHalfRadian;
protected:
	INT32						m_FrameCounter;
	CustomType::Matrix4x4	m_ViewMatrix[2];
	CustomType::Matrix4x4	m_ProjectionMatrix[2];
};
*/