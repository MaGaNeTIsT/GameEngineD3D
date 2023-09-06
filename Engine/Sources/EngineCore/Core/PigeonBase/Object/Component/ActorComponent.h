#pragma once

#include <CoreMinimal.h>

#include "Component.h"

namespace PigeonEngine
{

	class PActor;
	class PActorComponent : public PComponent
	{
	public:
		virtual void Init() override;
		virtual void Uninit() override;
	public:
		virtual BOOL32 ContainTransform()const;

		CLASS_VIRTUAL_NOCOPY_BODY(PActorComponent)

	public:
		PActor* GetOwnerActor()const;
		void    RemoveFromOwnerActor();
	protected:
		void    SetOwnerActor(PActor* NewOwner);
	private:
		PActor* Owner = nullptr;

	public:
		virtual void Destroy() override;
	public:
		// Render proxy functions START
		virtual void CreateRenderState() {}
		virtual void DestroyRenderState() {}
		virtual void SendUpdateRenderState() {}
		// Render proxy functions END
	};

};