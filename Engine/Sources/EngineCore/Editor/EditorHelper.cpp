#include "EditorHelper.h"
#include <PigeonBase/Object/Actor.h>
#include <PigeonBase/Object/Component/SceneComponent.h>
#include <PigeonBase/Object/Component/CameraAndLight/CameraComponent.h>
#include <RenderConfig/RenderConfig.h>
#if _EDITOR_ONLY
#include <RenderProxy/RenderSingletonObject.h>
#endif

namespace PigeonEngine
{

	PActor* SelectObjectInViewport(const PCameraComponent* InCamera, TArray<PActor*>& InActors, const FLOAT InMouseX, const FLOAT InMouseY, const ERect& InScreenRect)
	{
		const UINT32 ActorNum = InActors.Num<UINT32>();
		const FLOAT UsedMouseX = InMouseX;
		const FLOAT UsedMouseY = InMouseY;
		if ((!InCamera) || (ActorNum == 0u) || (InScreenRect.Right <= InScreenRect.Left) || (InScreenRect.Bottom <= InScreenRect.Top))
		{
			return nullptr;
		}
		if ((UsedMouseX < InScreenRect.Left) || (UsedMouseX > InScreenRect.Right) || (UsedMouseY < InScreenRect.Top) || (UsedMouseY > InScreenRect.Bottom))
		{
			return nullptr;
		}

		Vector3 CameraLocation(InCamera->GetComponentWorldLocation());
		const EViewport& UsedViewport = InCamera->GetCameraViewInfo().Viewport;
		EFrustum UsedFrustum(InCamera->GetCameraFrustum());

		TArray<Vector2> UsedFrustumProjection;
		{
			UsedFrustum.GeneratePlaneWorldSpace(
				CameraLocation,
				InCamera->GetComponentWorldRotation()
			);
			UsedFrustum.GenerateSeparatingProjectionWorldSpace();
			UsedFrustumProjection.Add(UsedFrustum.SeparateProjectionX);
			UsedFrustumProjection.Add(UsedFrustum.SeparateProjectionY);
			UsedFrustumProjection.Add(UsedFrustum.SeparateProjectionZ);
		}

		TArray<Vector3> SeparateAxis;
		{
			SeparateAxis.Add(Vector3::XVector());
			SeparateAxis.Add(Vector3::YVector());
			SeparateAxis.Add(Vector3::ZVector());
		}
		const UINT32 SeparateAxisNum = SeparateAxis.Num<UINT32>();
		TArray<FLOAT> CameraProjection;
		for (UINT32 SeparateAxisIndex = 0u; SeparateAxisIndex < SeparateAxisNum; SeparateAxisIndex++)
		{
			CameraProjection.Add(Vector3::Dot(SeparateAxis[SeparateAxisIndex], CameraLocation));
		}

		TArray<PActor*> TargetActors;
		TArray<FLOAT> TargetDepths;
		{
			Vector2 MouseScreenCoord(UsedMouseX - InScreenRect.Left, UsedMouseY - InScreenRect.Top);
			TargetActors.Reserve(ActorNum);
			TargetDepths.Reserve(ActorNum);
			for (UINT32 i = 0u; i < ActorNum; i++)
			{
				EBoundAABB TempActorBound(InActors[i]->GetBounds());
				if (!(TempActorBound.IsValid))
				{
					continue;
				}
				TArray<Vector3> TempBoundPoints;
				TempActorBound.CalculateBoundWithSpace(
					InActors[i]->GetActorWorldLocation(),
					InActors[i]->GetActorWorldRotation(),
					InActors[i]->GetActorWorldScale(),
					TempBoundPoints
				);
				TArray<Vector2> TempProjection;
				EBoundAABB::CalculateSeparatingProjectionWorldSpace(
					TempBoundPoints,
					SeparateAxis,
					TempProjection
				);
				if (!(EBoundAABB::IsProjectionOverlap(UsedFrustumProjection, TempProjection)))
				{
					continue;
				}
				BOOL32 IsOverlapNear = TRUE;
				for (UINT32 SeparateAxisIndex = 0u; SeparateAxisIndex < SeparateAxisNum; SeparateAxisIndex++)
				{
					if (IsOverlapNear)
					{
						if ((CameraProjection[SeparateAxisIndex] < TempProjection[SeparateAxisIndex].x) || (CameraProjection[SeparateAxisIndex] > TempProjection[SeparateAxisIndex].y))
						{
							IsOverlapNear = FALSE;
						}
					}
				}
				if (IsOverlapNear)
				{
					continue;
				}
				Vector3 ActorScreenAABB[2] =
				{
					Vector3(PE_FLOAT32_MAX, PE_FLOAT32_MAX, RCommonSettings::RENDER_DEPTH_MAX),
					Vector3(-PE_FLOAT32_MAX, -PE_FLOAT32_MAX, RCommonSettings::RENDER_DEPTH_MIN)
				};
				for (UINT32 BoundPointIndex = 0u; BoundPointIndex < 8u; BoundPointIndex++)
				{
					Vector3 TempScreenCoord;
					if (!(InCamera->TransformWorldToScreen(TempBoundPoints[BoundPointIndex], TempScreenCoord)))
					{
						TempScreenCoord.z = RCommonSettings::RENDER_DEPTH_MAX;
						if (TempScreenCoord.x < UsedViewport.TopLeftX)
						{
							TempScreenCoord.x = UsedViewport.TopLeftX;
						}
						else if (TempScreenCoord.x > (UsedViewport.TopLeftX + UsedViewport.Width))
						{
							TempScreenCoord.x = UsedViewport.TopLeftX + UsedViewport.Width;
						}
						if (TempScreenCoord.y < UsedViewport.TopLeftY)
						{
							TempScreenCoord.y = UsedViewport.TopLeftY;
						}
						else if (TempScreenCoord.y > (UsedViewport.TopLeftY + UsedViewport.Height))
						{
							TempScreenCoord.y = UsedViewport.TopLeftY + UsedViewport.Height;
						}
					}
					ActorScreenAABB[0] = MinVector3(TempScreenCoord, ActorScreenAABB[0]);
					ActorScreenAABB[1] = MaxVector3(TempScreenCoord, ActorScreenAABB[1]);
				}
				if ((MouseScreenCoord.x >= ActorScreenAABB[0].x) &&
					(MouseScreenCoord.x <= ActorScreenAABB[1].x) &&
					(MouseScreenCoord.y >= ActorScreenAABB[0].y) &&
					(MouseScreenCoord.y <= ActorScreenAABB[1].y))
				{
					TargetActors.Add(InActors[i]);
					TargetDepths.Add(ActorScreenAABB[0].z);
				}
			}
		}

		PActor* Result = nullptr;
		if (const UINT32 TargetActorNum = TargetActors.Num<UINT32>(); TargetActorNum > 0u)
		{
			Result = TargetActors[0];
			FLOAT CurrentDepth = RCommonSettings::RENDER_DEPTH_MAX;
			for (UINT32 TargetActorIndex = 0u; TargetActorIndex < TargetActorNum; TargetActorIndex++)
			{
				if (TargetDepths[TargetActorIndex] < CurrentDepth)
				{
					CurrentDepth = TargetDepths[TargetActorIndex];
					Result = TargetActors[TargetActorIndex];
				}
			}
		}

		return Result;
	}

	PSceneComponent* SelectObjectInViewport(const PCameraComponent* InCamera, TArray<PSceneComponent*>& InComponents, const FLOAT InMouseX, const FLOAT InMouseY, const ERect& InScreenRect)
	{
		const UINT32 ComponentNum = InComponents.Num<UINT32>();
		const FLOAT UsedMouseX = InMouseX;
		const FLOAT UsedMouseY = InMouseY;
		
		if ((!InCamera) || (ComponentNum == 0u) || (InScreenRect.Right <= InScreenRect.Left) || (InScreenRect.Bottom <= InScreenRect.Top))
		{
			return nullptr;
		}
		if ((UsedMouseX < InScreenRect.Left) || (UsedMouseX > InScreenRect.Right) || (UsedMouseY < InScreenRect.Top) || (UsedMouseY > InScreenRect.Bottom))
		{
			return nullptr;
		}

		Vector3 CameraLocation(InCamera->GetComponentWorldLocation());
		const EViewport& UsedViewport = InCamera->GetCameraViewInfo().Viewport;
		EFrustum UsedFrustum(InCamera->GetCameraFrustum());

		TArray<Vector2> UsedFrustumProjection;
		{
			UsedFrustum.GeneratePlaneWorldSpace(
				CameraLocation,
				InCamera->GetComponentWorldRotation()
			);
			UsedFrustum.GenerateSeparatingProjectionWorldSpace();
			UsedFrustumProjection.Add(UsedFrustum.SeparateProjectionX);
			UsedFrustumProjection.Add(UsedFrustum.SeparateProjectionY);
			UsedFrustumProjection.Add(UsedFrustum.SeparateProjectionZ);
		}

		TArray<Vector3> SeparateAxis;
		{
			SeparateAxis.Add(Vector3::XVector());
			SeparateAxis.Add(Vector3::YVector());
			SeparateAxis.Add(Vector3::ZVector());
		}
		const UINT32 SeparateAxisNum = SeparateAxis.Num<UINT32>();
		TArray<FLOAT> CameraProjection;
		for (UINT32 SeparateAxisIndex = 0u; SeparateAxisIndex < SeparateAxisNum; SeparateAxisIndex++)
		{
			CameraProjection.Add(Vector3::Dot(SeparateAxis[SeparateAxisIndex], CameraLocation));
		}

		TArray<PSceneComponent*> TargetComponents;
		TArray<FLOAT> TargetDepths;
		{
			Vector2 MouseScreenCoord(UsedMouseX - InScreenRect.Left, UsedMouseY - InScreenRect.Top);
			TargetComponents.Reserve(ComponentNum);
			TargetDepths.Reserve(ComponentNum);
			for (UINT32 i = 0u; i < ComponentNum; i++)
			{
				EBoundAABB TempComponentBound(InComponents[i]->GetLocalBound());
				if (!(TempComponentBound.IsValid))
				{
					continue;
				}
				TArray<Vector3> TempBoundPoints;
				TempComponentBound.CalculateBoundWithSpace(
					InComponents[i]->GetComponentWorldLocation(),
					InComponents[i]->GetComponentWorldRotation(),
					InComponents[i]->GetComponentWorldScale(),
					TempBoundPoints
				);
				TArray<Vector2> TempProjection;
				EBoundAABB::CalculateSeparatingProjectionWorldSpace(
					TempBoundPoints,
					SeparateAxis,
					TempProjection
				);
				if (!(EBoundAABB::IsProjectionOverlap(UsedFrustumProjection, TempProjection)))
				{
					continue;
				}
				BOOL32 IsOverlapNear = TRUE;
				for (UINT32 SeparateAxisIndex = 0u; SeparateAxisIndex < SeparateAxisNum; SeparateAxisIndex++)
				{
					if (IsOverlapNear)
					{
						if ((CameraProjection[SeparateAxisIndex] < TempProjection[SeparateAxisIndex].x) || (CameraProjection[SeparateAxisIndex] > TempProjection[SeparateAxisIndex].y))
						{
							IsOverlapNear = FALSE;
						}
					}
				}
				if (IsOverlapNear)
				{
					continue;
				}
				Vector3 ComponentScreenAABB[2] =
				{
					Vector3(PE_FLOAT32_MAX, PE_FLOAT32_MAX, RCommonSettings::RENDER_DEPTH_MAX),
					Vector3(-PE_FLOAT32_MAX, -PE_FLOAT32_MAX, RCommonSettings::RENDER_DEPTH_MIN)
				};
				for (UINT32 BoundPointIndex = 0u; BoundPointIndex < 8u; BoundPointIndex++)
				{
					Vector3 TempScreenCoord;
					if (!(InCamera->TransformWorldToScreen(TempBoundPoints[BoundPointIndex], TempScreenCoord)))
					{
						TempScreenCoord.z = RCommonSettings::RENDER_DEPTH_MAX;
						if (TempScreenCoord.x < UsedViewport.TopLeftX)
						{
							TempScreenCoord.x = UsedViewport.TopLeftX;
						}
						else if (TempScreenCoord.x > (UsedViewport.TopLeftX + UsedViewport.Width))
						{
							TempScreenCoord.x = UsedViewport.TopLeftX + UsedViewport.Width;
						}
						if (TempScreenCoord.y < UsedViewport.TopLeftY)
						{
							TempScreenCoord.y = UsedViewport.TopLeftY;
						}
						else if (TempScreenCoord.y > (UsedViewport.TopLeftY + UsedViewport.Height))
						{
							TempScreenCoord.y = UsedViewport.TopLeftY + UsedViewport.Height;
						}
					}
					ComponentScreenAABB[0] = MinVector3(TempScreenCoord, ComponentScreenAABB[0]);
					ComponentScreenAABB[1] = MaxVector3(TempScreenCoord, ComponentScreenAABB[1]);
				}
				if ((MouseScreenCoord.x >= ComponentScreenAABB[0].x) &&
					(MouseScreenCoord.x <= ComponentScreenAABB[1].x) &&
					(MouseScreenCoord.y >= ComponentScreenAABB[0].y) &&
					(MouseScreenCoord.y <= ComponentScreenAABB[1].y))
				{
					TargetComponents.Add(InComponents[i]);
					TargetDepths.Add(ComponentScreenAABB[0].z);
				}
			}
		}

		PSceneComponent* Result = nullptr;
		if (const UINT32 TargetComponentNum = TargetComponents.Num<UINT32>(); TargetComponentNum > 0u)
		{
			Result = TargetComponents[0];
			FLOAT CurrentDepth = RCommonSettings::RENDER_DEPTH_MAX;
			for (UINT32 TargetComponentIndex = 0u; TargetComponentIndex < TargetComponentNum; TargetComponentIndex++)
			{
				if (TargetDepths[TargetComponentIndex] < CurrentDepth)
				{
					CurrentDepth = TargetDepths[TargetComponentIndex];
					Result = TargetComponents[TargetComponentIndex];
				}
			}
		}

		return Result;
	}

#if _EDITOR_ONLY
	void DrawObjectBounds(TArray<PActor*>& InObjects)
	{
		if (const UINT32 ObjectNum = InObjects.Num<UINT32>(); ObjectNum > 0u)
		{
			const Color4 DebugColor((FLOAT)(0x99) / 255.0f, (FLOAT)(0x32) / 255.0f, (FLOAT)(0xcd) / 255.0f);
			RDebugWireframePrimitiveManager* DebugPrimitiveMangaer = RDebugWireframePrimitiveManager::GetManagerSingleton();
			for (UINT32 i = 0u; i < ObjectNum; i++)
			{
				EBoundAABB TempBound(InObjects[i]->GetBounds());
				if (!(TempBound.IsValid))
				{
					continue;
				}
				const Vector3		BoundWorldLocation(InObjects[i]->GetActorWorldLocation());
				const Quaternion	BoundWorldRotation(InObjects[i]->GetActorWorldRotation());
				const Vector3		BoundWorldScaling(InObjects[i]->GetActorWorldScale());
				TArray<Vector3> BoundWorldPoints;
				TempBound.CalculateBoundWithSpace(
					BoundWorldLocation,
					BoundWorldRotation,
					BoundWorldScaling,
					BoundWorldPoints
				);
				const Vector3 BoundExtent(
					(BoundWorldPoints[1] - BoundWorldPoints[0]).Length(),
					(BoundWorldPoints[0] - BoundWorldPoints[4]).Length(),
					(BoundWorldPoints[0] - BoundWorldPoints[2]).Length()
				);
				Vector3 BoundCenter(0.f, 0.f, 0.f);
				{
					const UINT32 PointNum = BoundWorldPoints.Num<UINT32>();
					for (UINT32 PointIndex = 0u; PointIndex < PointNum; PointIndex++)
					{
						BoundCenter += BoundWorldPoints[PointIndex];
					}
					BoundCenter /= (FLOAT)PointNum;
				}
				DebugPrimitiveMangaer->DrawCuboid(BoundCenter, BoundWorldRotation,
					BoundExtent.x, BoundExtent.y, BoundExtent.z,
					DebugColor);
			}
		}
	}
#endif

};