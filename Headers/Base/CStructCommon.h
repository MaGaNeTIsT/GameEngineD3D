#pragma once

#include "../../Entry/MyMain.h"

class CTexture2D;

namespace CustomStruct
{
	struct CColor
	{
		CColor() { r = 1.f; g = 1.f; b = 1.f; a = 1.f; }
		CColor(const FLOAT& _r, const FLOAT& _g, const FLOAT& _b, const FLOAT& _a)
		{
			r = _r; g = _g;
			b = _b; a = _a;
		}
		FLOAT	r;
		FLOAT	g;
		FLOAT	b;
		FLOAT	a;
	};

	struct CLightData
	{
		CLightData() { ZeroMemory(this, sizeof(*this)); }
		XMFLOAT4	Direction;
		CColor		Color;
	};

	struct ConstantBufferPerFrame
	{
		ConstantBufferPerFrame() { ZeroMemory(this, sizeof(*this)); }
		XMFLOAT4X4	ViewMatrix;
		XMFLOAT4X4	ViewInvMatrix;
		XMFLOAT4X4	ProjectionMatrix;
		XMFLOAT4X4	ProjectionInvMatrix;
		XMFLOAT4X4	ViewProjectionMatrix;
		XMFLOAT4X4	ViewProjectionInvMatrix;
		XMFLOAT4	TimeParams;
		XMFLOAT4	DepthMultiAdd;
		XMFLOAT4	ScreenToViewSpaceParams;
		XMFLOAT4	CameraViewportSizeAndInvSize;
		XMFLOAT4	CameraViewportRect;
		XMFLOAT3	CameraWorldPosition;
		FLOAT		DirectionalLightCount;
		CLightData	DirectionalLightData[4];
	};

	struct ConstantBufferPerDraw
	{
		ConstantBufferPerDraw() { ZeroMemory(this, sizeof(*this)); }
		XMFLOAT4X4	WorldMatrix;
		XMFLOAT4X4	WorldInvMatrix;
		XMFLOAT4X4	WorldInvTransposeMatrix;
		XMFLOAT4	_CustomParameter;
	};

	struct CVertex3D
	{
		CVertex3D() { ZeroMemory(this, sizeof(*this)); }
		XMFLOAT4	Position;
		XMFLOAT4	Normal;
		XMFLOAT4	Tangent;
		XMFLOAT4	Color;
		XMFLOAT2	UV0;
	};

	struct CSubMeshInfo
	{
		CSubMeshInfo() { ZeroMemory(this, sizeof(*this)); }
		UINT		VertexStart;
		UINT		VertexCount;
		UINT		IndexStart;
		UINT		IndexCount;
	};
}