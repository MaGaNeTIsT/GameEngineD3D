#pragma once

#include "../../EngineRender/RenderBase/Headers/CStructCommon.h"
#include "../../EngineGame/Headers/CGameObject.h"

class CTestModel : public CGameObject
{
public:
	virtual void	Init()override;
	virtual void	Uninit()override;
	virtual void	Update()override;
	virtual void	FixedUpdate()override;
	virtual void	Draw()override;
	virtual void	DrawExtra()override;
protected:
	virtual void	PrepareDraw()override;
public:
	CTestModel();
	virtual ~CTestModel();
protected:
	class CTexture2D*	m_AlbedoTexture;
	class CTexture2D*	m_NormalTexture;
	class CTexture2D*	m_PropertyTexture;
};