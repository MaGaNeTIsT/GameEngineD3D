﻿#pragma once
#include <Main.h>
#include <set>

#include "./ActorComponent.h"
#include "./Transform.h"

namespace PigeonEngine
{

    class PSceneComponent : public PActorComponent
    {

		EClass(PSceneComponent, PActorComponent)

	public:

	public:
		virtual BOOL ContainTransform()const override;

    	Vector3    GetComponentLocation() const;
    	Quaternion GetComponentRotation() const;
    	Vector3    GetComponentScale() const;

    	Vector3    GetComponentWorldLocation() const;
    	Quaternion GetComponentWorldRotation() const;
    	Vector3    GetComponentWorldScale() const;
    	
    	const ETransform* GetTransform()const ;

	protected:
        ETransform	Transform;
		

        CLASS_VIRTUAL_NOCOPY_BODY(PSceneComponent)

    public:
    	// WIP
    	// 1 attach rules
    	// 2 relative transform

    	// Attach this component to another component
    	void AttachToComponent(PSceneComponent* AttachTo);
    	// Attach another component to this
    	void AttachComponentTo(PSceneComponent* Component);
    	static void AttachComponentToComponent(PSceneComponent* Component, PSceneComponent* AttachTo);
    	PSceneComponent* GetAttachedParentComponent()const;
    private:
    	PSceneComponent* AttachedParentComponent = nullptr;
    	std::set<PSceneComponent*> ChildrenComponents;
    };

};

