#include "MovementComponent.h"
#include "../../../../../../../EngineThirdParty/JoltPhysics/Headers/Shapes.h"
#include "../../../../../../../EngineThirdParty/JoltPhysics/Headers/PhysicsManager.h"
#include "../../../../../../../EngineThirdParty/JoltPhysics/Headers/Character/Character.h"
#include "../../../../../../../EngineThirdParty/JoltPhysics/Headers/Character/CharacterBase.h"
#include "Base/Math/Math.h"

namespace PigeonEngine
{
	PMovementComponent::~PMovementComponent()
	{
	}

	void PMovementComponent::Init()
	{
	}

	void PMovementComponent::Uninit()
	{
	}

	void PMovementComponent::HandleInput(const ECharacterMoveInput& Input)
	{
		Vector3 MovementDirection = m_Character->GetActorForwardVector() * EMath::Clamp(Input.YInput, 0.f, 1.f) + m_Character->GetActorRightVector() * EMath::Clamp(Input.XInput, 0.f, 1.f);
		MovementDirection.Normalize();
		HandleInputInternal(MovementDirection, Input.bJump, Input.bCrouch, Input.bRun);
	}

	void PMovementComponent::HandleInputInternal(Vector3 InMovementDirection, BOOL32 InJump, BOOL32 InSwitchStance, BOOL32 InRun)
	{
		FCharacter* Character = m_Character->GetPhysicsCharacter();
		//character->SetPositionAndRotation(m_Character->GetActorLocation(), m_Character->GetActorRotation());
		FCharacterBase::EGroundState GroundState = Character->GetGroundState();
		if (GroundState == FCharacterBase::EGroundState::OnSteepGround
			|| GroundState == FCharacterBase::EGroundState::NotSupported)
		{
			Vector3 Normal = Character->GetGroundNormal();
			Normal.y = (0.0f);
			float Dot = Normal.Dot(InMovementDirection);
			if (Dot < 0.0f)
				InMovementDirection -= (Dot * Normal) / Normal.LengthSquare();
		}

		// Stance switch
		if (InSwitchStance)
			Character->SetShape(Character->GetShape() == m_Character->GetStandingShape() ? m_Character->GetCrouchingShape() : m_Character->GetStandingShape(), 1.5f * 0.02f);

		if (m_Character->ControlMovementDuringJump || Character->IsSupported())
		{
			// Update velocity
			Vector3 CurrentVelocity = Character->GetLinearVelocity();
			Vector3 DesiredVelocity = InRun ? m_Character->CharacterRunSpeed : m_Character->CharacterSpeed * InMovementDirection;
			DesiredVelocity.y = CurrentVelocity.y;
			Vector3 NewVelocity = 0.75f * CurrentVelocity + 0.25f * DesiredVelocity;

			// Jump
			if (InJump && GroundState == FCharacterBase::EGroundState::OnGround)
				NewVelocity += Vector3(0.f, m_Character->JumpSpeed, 0.f);

			// Update the velocity
			Character->SetLinearVelocity(NewVelocity);
		}

		m_Character->SetActorLocation(Character->GetPosition());
		m_Character->SetActorRotation(Character->GetRotation());
	}
}