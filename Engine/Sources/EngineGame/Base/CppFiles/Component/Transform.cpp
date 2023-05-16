#include "../../Headers/Component/Transform.h"
#include "../../Headers/Component/ActorComponent.h"
#include "../../Headers/Component/SceneComponent.h"

namespace PigeonEngine
{

	ETransform::ETransform() : LocalLocation(Vector3::Zero()), LocalRotation(Quaternion::Identity()), LocalScaling(Vector3::One())
	{
	}
	ETransform::~ETransform()
	{
	}
	ETransform::ETransform(const ETransform& other) : LocalLocation(other.LocalLocation), LocalRotation(other.LocalRotation), LocalScaling(other.LocalScaling)
	{
	}
	ETransform& ETransform::operator=(const ETransform& other)
	{
		LocalLocation = other.LocalLocation;
		LocalRotation = other.LocalRotation;
		LocalScaling = other.LocalScaling;
		return (*this);
	}
	template<ECoordinateSpaceType _CoordinateSpaceType>
	Matrix4x4 ETransform::ToMatrix4x4(const PSceneComponent* InParentComponent)const
	{
		if (_CoordinateSpaceType == ECoordinateSpaceType::ECST_LOCAL)
		{
			return (Matrix4x4(LocalLocation, LocalRotation, LocalScaling));
		}
		else if (_CoordinateSpaceType == ECoordinateSpaceType::ECST_WORLD)
		{
			return (Matrix4x4(
				GetWorldLocationInternal(InParentComponent),
				GetWorldRotationInternal(InParentComponent),
				GetWorldScalingInternal(InParentComponent)));
		}
		else
		{
			PE_FAILED(ENGINE_RENDER_CORE_ERROR, "The function of ETransform::ToMatrix4x4 enum check coordinate space type failed.");
			return (Matrix4x4::Identity());
		}
	}
	template<ECoordinateSpaceType _CoordinateSpaceType>
	Vector3 ETransform::GetLocation(const PSceneComponent* InParentComponent)const
	{
		if (_CoordinateSpaceType == ECoordinateSpaceType::ECST_LOCAL)
		{
			return LocalLocation;
		}
		else if (_CoordinateSpaceType == ECoordinateSpaceType::ECST_WORLD)
		{
			return (GetWorldLocationInternal(InParentComponent));
		}
		else
		{
			PE_FAILED(ENGINE_RENDER_CORE_ERROR, "The function of ETransform::GetLocation enum check coordinate space type failed.");
			return (Vector3::Zero());
		}
	}
	template<ECoordinateSpaceType _CoordinateSpaceType>
	Quaternion ETransform::GetRotation(const PSceneComponent* InParentComponent)const
	{
		if (_CoordinateSpaceType == ECoordinateSpaceType::ECST_LOCAL)
		{
			return LocalRotation;
		}
		else if (_CoordinateSpaceType == ECoordinateSpaceType::ECST_WORLD)
		{
			return (GetWorldRotationInternal(InParentComponent));
		}
		else
		{
			PE_FAILED(ENGINE_RENDER_CORE_ERROR, "The function of ETransform::GetRotation enum check coordinate space type failed.");
			return (Quaternion::Identity());
		}
	}
	template<ECoordinateSpaceType _CoordinateSpaceType>
	Vector3 ETransform::GetScaling(const PSceneComponent* InParentComponent)const
	{
		if (_CoordinateSpaceType == ECoordinateSpaceType::ECST_LOCAL)
		{
			return LocalScaling;
		}
		else if (_CoordinateSpaceType == ECoordinateSpaceType::ECST_WORLD)
		{
			return (GetWorldScalingInternal(InParentComponent));
		}
		else
		{
			PE_FAILED(ENGINE_RENDER_CORE_ERROR, "The function of ETransform::GetScaling enum check coordinate space type failed.");
			return (Vector3::One());
		}
	}
	template<ECoordinateSpaceType _CoordinateSpaceType>
	Vector3 ETransform::GetForwardVector(const PSceneComponent* InParentComponent)const
	{
		if (_CoordinateSpaceType == ECoordinateSpaceType::ECST_LOCAL)
		{
			return (LocalRotation.MultiplyVector(Vector3::ZVector()));
		}
		else if (_CoordinateSpaceType == ECoordinateSpaceType::ECST_WORLD)
		{
			return (GetForwardVectorInternal(InParentComponent));
		}
		else
		{
			PE_FAILED(ENGINE_RENDER_CORE_ERROR, "The function of ETransform::GetForwardVector enum check coordinate space type failed.");
			return (Vector3::ZVector());
		}
	}
	template<ECoordinateSpaceType _CoordinateSpaceType>
	Vector3 ETransform::GetUpVector(const PSceneComponent* InParentComponent)const
	{
		if (_CoordinateSpaceType == ECoordinateSpaceType::ECST_LOCAL)
		{
			return (LocalRotation.MultiplyVector(Vector3::YVector()));
		}
		else if (_CoordinateSpaceType == ECoordinateSpaceType::ECST_WORLD)
		{
			return (GetUpVectorInternal(InParentComponent));
		}
		else
		{
			PE_FAILED(ENGINE_RENDER_CORE_ERROR, "The function of ETransform::GetUpVector enum check coordinate space type failed.");
			return (Vector3::YVector());
		}
	}
	template<ECoordinateSpaceType _CoordinateSpaceType>
	Vector3 ETransform::GetRightVector(const PSceneComponent* InParentComponent)const
	{
		if (_CoordinateSpaceType == ECoordinateSpaceType::ECST_LOCAL)
		{
			return (LocalRotation.MultiplyVector(Vector3::XVector()));
		}
		else if (_CoordinateSpaceType == ECoordinateSpaceType::ECST_WORLD)
		{
			return (GetRightVectorInternal(InParentComponent));
		}
		else
		{
			PE_FAILED(ENGINE_RENDER_CORE_ERROR, "The function of ETransform::GetRightVector enum check coordinate space type failed.");
			return (Vector3::XVector());
		}
	}
	Matrix4x4 ETransform::GetLocalToWorldMatrix(const PSceneComponent* InParentComponent)const
	{
		return Matrix4x4(
			GetWorldLocationInternal(InParentComponent),
			GetWorldRotationInternal(InParentComponent),
			GetWorldScalingInternal(InParentComponent));
	}
	Matrix4x4 ETransform::GetWorldToLocalMatrix(const PSceneComponent* InParentComponent)const
	{
		Matrix4x4 Result(
			GetWorldLocationInternal(InParentComponent),
			GetWorldRotationInternal(InParentComponent),
			GetWorldScalingInternal(InParentComponent));
		return (Result.Inverse());
	}
	Vector3 ETransform::GetForwardVectorInternal(const PSceneComponent* InParentComponent)const
	{
		Quaternion WorldRot(LocalRotation);
		GetWorldRotationInternal(WorldRot, InParentComponent);
		return (WorldRot.MultiplyVector(Vector3::ZVector()));
	}
	Vector3 ETransform::GetUpVectorInternal(const PSceneComponent* InParentComponent)const
	{
		Quaternion WorldRot(LocalRotation);
		GetWorldRotationInternal(WorldRot, InParentComponent);
		return (WorldRot.MultiplyVector(Vector3::YVector()));
	}
	Vector3 ETransform::GetRightVectorInternal(const PSceneComponent* InParentComponent)const
	{
		Quaternion WorldRot(LocalRotation);
		GetWorldRotationInternal(WorldRot, InParentComponent);
		return (WorldRot.MultiplyVector(Vector3::XVector()));
	}
	Vector3 ETransform::GetWorldLocationInternal(const PSceneComponent* InParentComponent)const
	{
		Vector3 Result(LocalLocation);
		GetWorldLocationInternal(Result, InParentComponent);
		return Result;
	}
	Quaternion ETransform::GetWorldRotationInternal(const PSceneComponent* InParentComponent)const
	{
		Quaternion Result(LocalRotation);
		GetWorldRotationInternal(Result, InParentComponent);
		return Result;
	}
	Vector3 ETransform::GetWorldScalingInternal(const PSceneComponent* InParentComponent)const
	{
		Vector3 Result(LocalScaling);
		GetWorldScalingInternal(Result, InParentComponent);
		return Result;
	}
	void ETransform::GetWorldLocationInternal(Vector3& InOutWorldLocation, const PSceneComponent* InParentComponent)const
	{
		if (!InParentComponent)
		{
			return;
		}
		if (const ETransform* ParentTransform = InParentComponent->GetTransform(); ParentTransform)
		{
			InOutWorldLocation += ParentTransform->LocalLocation;
		}
		GetWorldLocationInternal(InOutWorldLocation, InParentComponent->GetAttachedParentComponent());
	}
	void ETransform::GetWorldRotationInternal(Quaternion& InOutWorldRotation, const PSceneComponent* InParentComponent)const
	{
		if (!InParentComponent)
		{
			return;
		}
		if (const ETransform* ParentTransform = InParentComponent->GetTransform(); ParentTransform)
		{
			InOutWorldRotation = ParentTransform->LocalRotation * InOutWorldRotation;
		}
		GetWorldRotationInternal(InOutWorldRotation, InParentComponent->GetAttachedParentComponent());
	}
	void ETransform::GetWorldScalingInternal(Vector3& InOutWorldScaling, const PSceneComponent* InParentComponent)const
	{
		if (!InParentComponent)
		{
			return;
		}
		if (const ETransform* ParentTransform = InParentComponent->GetTransform(); ParentTransform)
		{
			InOutWorldScaling *= ParentTransform->LocalScaling;
		}
		GetWorldScalingInternal(InOutWorldScaling, InParentComponent->GetAttachedParentComponent());
	}

};