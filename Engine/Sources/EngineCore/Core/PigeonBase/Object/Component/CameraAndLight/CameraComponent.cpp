#include "CameraComponent.h"
#include <RenderProxy/ViewProxy.h>
#include <PigeonBase/Object/World/World.h>
#include <Renderer/RenderInterface.h>

namespace PigeonEngine
{

	static void RegisterClassTypes()
	{
		RegisterClassType<PCameraComponent, PSceneComponent>();
	}

	PE_REGISTER_CLASS_TYPE(&RegisterClassTypes);

	PCameraComponent::PCameraComponent(FLOAT InViewportLeftTopX, FLOAT InViewportLeftTopY, FLOAT InViewportWidth, FLOAT InViewportHeight, FLOAT InFovAngleY, FLOAT InFarDist, FLOAT InNearDist)
		: CameraViewInfo(ECameraViewInfo(InViewportLeftTopX, InViewportLeftTopY, InViewportWidth, InViewportHeight, InFovAngleY, InFarDist, InNearDist)), ViewProxy(nullptr), UpdateState(PCameraUpdateState::CAMERA_UPDATE_STATE_NONE)
	{
		CameraMatrix.GeneratePerspectiveProjectPart(CameraViewInfo.Viewport, CameraViewInfo.FovAngleY, CameraViewInfo.NearDist, CameraViewInfo.FarDist);
		CameraMatrix.GenerateViewPart(GetComponentWorldLocation(), GetComponentWorldRotation());
		CameraMatrix.GenerateFinalMatrix();

		CameraFrustum.GeneratePerspectiveFrustumInfo(CameraViewInfo.FovAngleY, CameraViewInfo.Viewport.Width / CameraViewInfo.Viewport.Height, CameraViewInfo.NearDist, CameraViewInfo.FarDist);
	}
	PCameraComponent::PCameraComponent()
		: ViewProxy(nullptr), UpdateState(PCameraUpdateState::CAMERA_UPDATE_STATE_NONE)
	{
	}
	PCameraComponent::~PCameraComponent()
	{
	}
	void PCameraComponent::SetCameraViewInfo(FLOAT InTopLeftX, FLOAT InTopLeftY, FLOAT InWidth, FLOAT InHeight, FLOAT InFovAngleY, FLOAT InFarDist, FLOAT InNearDist)
	{
		CameraViewInfo.Viewport.TopLeftX	= InTopLeftX;
		CameraViewInfo.Viewport.TopLeftY	= InTopLeftY;
		CameraViewInfo.Viewport.Width		= InWidth;
		CameraViewInfo.Viewport.Height		= InHeight;
		CameraViewInfo.Viewport.MinDepth	= RCommonSettings::RENDER_DEPTH_MIN;
		CameraViewInfo.Viewport.MaxDepth	= RCommonSettings::RENDER_DEPTH_MAX;
		CameraViewInfo.FovAngleY	= InFovAngleY;
		CameraViewInfo.FarDist		= InFarDist;
		CameraViewInfo.NearDist		= InNearDist;

		CameraMatrix.GeneratePerspectiveProjectPart(CameraViewInfo.Viewport, CameraViewInfo.FovAngleY, CameraViewInfo.NearDist, CameraViewInfo.FarDist);
		CameraMatrix.GenerateFinalMatrix();

		MarkAsDirty(PCameraUpdateState::CAMERA_UPDATE_STATE_VIEW);
		MarkAsDirty(PCameraUpdateState::CAMERA_UPDATE_STATE_MATRIX);
	}
	void PCameraComponent::SetViewport(FLOAT InTopLeftX, FLOAT InTopLeftY, FLOAT InWidth, FLOAT InHeight)
	{
		CameraViewInfo.Viewport.TopLeftX	= InTopLeftX;
		CameraViewInfo.Viewport.TopLeftY	= InTopLeftY;
		CameraViewInfo.Viewport.Width		= InWidth;
		CameraViewInfo.Viewport.Height		= InHeight;
		CameraViewInfo.Viewport.MinDepth = RCommonSettings::RENDER_DEPTH_MIN;
		CameraViewInfo.Viewport.MaxDepth = RCommonSettings::RENDER_DEPTH_MAX;

		CameraMatrix.GeneratePerspectiveProjectPart(CameraViewInfo.Viewport, CameraViewInfo.FovAngleY, CameraViewInfo.NearDist, CameraViewInfo.FarDist);
		CameraMatrix.GenerateFinalMatrix();

		MarkAsDirty(PCameraUpdateState::CAMERA_UPDATE_STATE_VIEW);
		MarkAsDirty(PCameraUpdateState::CAMERA_UPDATE_STATE_MATRIX);
	}
	void PCameraComponent::SetFov(FLOAT InFovAngleY)
	{
		CameraViewInfo.FovAngleY = InFovAngleY;

		CameraMatrix.GeneratePerspectiveProjectPart(CameraViewInfo.Viewport, CameraViewInfo.FovAngleY, CameraViewInfo.NearDist, CameraViewInfo.FarDist);
		CameraMatrix.GenerateFinalMatrix();

		MarkAsDirty(PCameraUpdateState::CAMERA_UPDATE_STATE_VIEW);
		MarkAsDirty(PCameraUpdateState::CAMERA_UPDATE_STATE_MATRIX);
	}
	Vector3 PCameraComponent::TransformScreenToWorld(const Vector3& InScreenCoordWithZ)const
	{
		Vector3 Result;
		CameraMatrix.TransformScreenPointToWorld(CameraViewInfo.Viewport, InScreenCoordWithZ, Result);
		return Result;
	}
	BOOL32 PCameraComponent::TransformWorldToScreen(const Vector3& InWorldLocation, Vector3& OutScreenCoordWithZ)const
	{
		return (CameraMatrix.TransformWorldPointToScreen(CameraViewInfo.Viewport, InWorldLocation, OutScreenCoordWithZ));
	}
	Vector3 PCameraComponent::TransformWorldToView(const Vector3& InWorldLocation)const
	{
		Vector3 Result;
		CameraMatrix.TransformWorldPointToView(InWorldLocation, Result);
		return Result;
	}
	Vector3 PCameraComponent::TransformViewToWorld(const Vector3& InViewLocation)const
	{
		Vector3 Result;
		CameraMatrix.TransformViewPointToWorld(InViewLocation, Result);
		return Result;
	}
	void PCameraComponent::UpdateCameraMatrix()
	{
		CameraMatrix.GenerateViewPart(GetComponentWorldLocation(), GetComponentWorldRotation());
		CameraMatrix.GenerateFinalMatrix();

		MarkAsDirty(PCameraUpdateState::CAMERA_UPDATE_STATE_MATRIX);
	}

	// Render proxy functions START
	RViewProxy* PCameraComponent::GetSceneProxy()
	{
		return ViewProxy;
	}
	const RViewProxy* PCameraComponent::GetSceneProxy()const
	{
		return ViewProxy;
	}
	RViewProxy* PCameraComponent::CreateSceneProxy()
	{
		Check((ENGINE_RENDER_CORE_ERROR), ("Try creating mesh scene proxy, but already exist scene proxy."), (!ViewProxy));
		ViewProxy = new RViewProxy(this);
		return ViewProxy;
	}
	UINT8 PCameraComponent::GetUpdateRenderState()const
	{
		return UpdateState;
	}
	void PCameraComponent::CreateRenderState()
	{
		PSceneComponent::CreateRenderState();
		if (ShouldRender())
		{
			PWorldManager::GetWorld()->GetRenderScene()->AddCamera(this);
		}
	}
	void PCameraComponent::DestroyRenderState()
	{
		PWorldManager::GetWorld()->GetRenderScene()->RemoveCamera(this);
		PSceneComponent::DestroyRenderState();
	}
	void PCameraComponent::SendUpdateRenderState()
	{
		if (ShouldRender() && IsRenderStateDirty())
		{
			PWorldManager::GetWorld()->GetRenderScene()->UpdateCamera(this);
		}
		PSceneComponent::SendUpdateRenderState();
	}
	void PCameraComponent::MarkAsDirty(PCameraUpdateState InState)
	{
		UpdateState |= InState;
		MarkRenderStateAsDirty();
	}
	void PCameraComponent::MarkRenderStateAsDirty()
	{
		PSceneComponent::MarkRenderStateAsDirty();
	}
	void PCameraComponent::CleanMarkRenderStateDirty()
	{
		UpdateState = PCameraUpdateState::CAMERA_UPDATE_STATE_NONE;
		PSceneComponent::CleanMarkRenderStateDirty();
	}
	// Render proxy functions END

};