#pragma once

#include <CoreMinimal.h>
#include <Base/RegisterBase.h>
#include "RenderInterface.h"
#include <RenderOctree.h>

namespace PigeonEngine
{

	class PCameraComponent;
	class PSkyLightComponent;
	class PDirectionalLightComponent;
	class PStaticMeshComponent;
	class PSkeletalMeshComponent;
	class RViewProxy;
	class RSkyLightSceneProxy;
	class RDirectionalLightSceneProxy;
	class RStaticMeshSceneProxy;
	class RSkeletalMeshSceneProxy;

	class RCommand final
	{
	public:
		template<typename _TFunctionType>
		void EnqueueCommand(_TFunctionType InFunction)
		{
			RawCommands.AddRegisterFunction<_TFunctionType>(InFunction);
		}
		void DoCommands()
		{
			RawCommands.DoRegister();
		}
		void EmptyQueue()
		{
			RawCommands.ClearRegister();
		}
	private:
		ERegisterBase<EVoidFunctionType>	RawCommands;
	public:
		RCommand() {}
		RCommand(const RCommand&) = delete;
		~RCommand() {}
		RCommand& operator=(const RCommand&) = delete;
	};

	template<typename _TProxyType>
	struct RSceneProxyMapping
	{
		RSceneProxyMapping() = default;
		RSceneProxyMapping(const RSceneProxyMapping&) = delete;
		RSceneProxyMapping& operator=(const RSceneProxyMapping&) = delete;

		/// <summary>
		/// Add scene proxy into mappings
		/// </summary>
		/// <param name="InSceneProxy">The scene proxy that needs to adding into mappings</param>
		/// <returns>If mappings already contains this scene proxy</returns>
		BOOL32 AddSceneProxy(_TProxyType* InSceneProxy)
		{
			if (!InSceneProxy)
			{
				return FALSE;
			}
			PE_CHECK((ENGINE_RENDER_CORE_ERROR), ("Check scene proxy mapping and array length failed."), (SceneProxyMapping.Num() == SceneProxies.Num()));
			const ObjectIdentityType& PrimitiveID = InSceneProxy->GetUniqueID();
			if (SceneProxyMapping.ContainsKey(PrimitiveID))
			{
				return FALSE;
			}
			SceneProxyMapping.Add(PrimitiveID, SceneProxies.Num<UINT32>());
			SceneProxies.Add(InSceneProxy);
			return TRUE;
		}

		/// <summary>
		/// Remove scene proxy from mappings(This operation will NOT delete the memory that scene proxy pointer pointed to)
		/// </summary>
		/// <param name="InSceneProxy">The scene proxy that needs to removing from mappings</param>
		/// <returns>If mappings removes scene proxy success</returns>
		BOOL32 RemoveSceneProxy(_TProxyType* InSceneProxy)
		{
			if ((!InSceneProxy) || (SceneProxies.Num() == 0))
			{
				return FALSE;
			}
			PE_CHECK((ENGINE_RENDER_CORE_ERROR), ("Check primitive mapping and array length failed."), (SceneProxyMapping.Num() == SceneProxies.Num()));
			const ObjectIdentityType& PrimitiveID = InSceneProxy->GetUniqueID();
			UINT32 PrimitiveIndex = (UINT32)(-1);
			if (!(SceneProxyMapping.FindValue(PrimitiveID, PrimitiveIndex)))
			{
				return FALSE;
			}
			PE_CHECK((ENGINE_RENDER_CORE_ERROR), ("Check primitive index failed."), (PrimitiveIndex < SceneProxies.Num<UINT32>()));
			if (const UINT32 LastIndex = SceneProxies.Num<UINT32>() - 1u; PrimitiveIndex != LastIndex)
			{
				_TProxyType* TempPrimitiveProxy = SceneProxies[LastIndex];
				SceneProxies[LastIndex] = SceneProxies[PrimitiveIndex];
				SceneProxies[PrimitiveIndex] = TempPrimitiveProxy;
				SceneProxyMapping[TempPrimitiveProxy->GetUniqueID()] = PrimitiveIndex;
			}
			SceneProxyMapping.Remove(PrimitiveID);
			SceneProxies.Pop();
			return TRUE;
		}
		UINT32 GetSceneProxyCount()const
		{
			PE_CHECK((ENGINE_RENDER_CORE_ERROR), ("Check primitive mapping and array length failed."), (SceneProxyMapping.Num() == SceneProxies.Num()));
			return (SceneProxies.Num<UINT32>());
		}

		TMap<ObjectIdentityType, UINT32>	SceneProxyMapping;
		TArray<_TProxyType*>				SceneProxies;
	};

	class RScene : public EObjectBase, public RSceneInterface
	{
	public:
		void			Init();
		void			Uninit();
		virtual void	ClearAll()override;
		virtual void	UnbindErrorCheck()override;
	public:
		virtual void	AddCamera(PCameraComponent* InComponent)override;
		virtual void	RemoveCamera(PCameraComponent* InComponent)override;
		virtual void	UpdateCamera(PCameraComponent* InComponent)override;

		virtual void	AddSkyLight(PSkyLightComponent* InComponent)override;
		virtual void	RemoveSkyLight(PSkyLightComponent* InComponent)override;
		virtual void	UpdateSkyLight(PSkyLightComponent* InComponent)override;

		virtual void	AddDirectionalLight(PDirectionalLightComponent* InComponent)override;
		virtual void	RemoveDirectionalLight(PDirectionalLightComponent* InComponent)override;
		virtual void	UpdateDirectionalLight(PDirectionalLightComponent* InComponent)override;

		virtual void	AddStaticMesh(PStaticMeshComponent* InComponent)override;
		virtual void	RemoveStaticMesh(PStaticMeshComponent* InComponent)override;
		virtual void	UpdateStaticMesh(PStaticMeshComponent* InComponent)override;
		virtual void	AddSkeletalMesh(PSkeletalMeshComponent* InComponent)override;
		virtual void	RemoveSkeletalMesh(PSkeletalMeshComponent* InComponent)override;
		virtual void	UpdateSkeletalMesh(PSkeletalMeshComponent* InComponent)override;
	public:
		RCommand&		GetAddCommands();
		const RCommand&	GetAddCommands()const;
		RCommand&		GetRemoveCommands();
		const RCommand&	GetRemoveCommands()const;
		RCommand&		GetUpdateCommands();
		const RCommand&	GetUpdateCommands()const;
		ROctree&		GetSceneOctree();
		const ROctree&	GetSceneOctree()const;

		RSceneProxyMapping<RViewProxy>&							GetViewProxies();
		const RSceneProxyMapping<RViewProxy>&					GetViewProxies()const;
		RSceneProxyMapping<RSkyLightSceneProxy>&				GetSkyLightProxies();
		const RSceneProxyMapping<RSkyLightSceneProxy>&			GetSkyLightProxies()const;
		RSceneProxyMapping<RDirectionalLightSceneProxy>&		GetDirectionalLightSceneProxies();
		const RSceneProxyMapping<RDirectionalLightSceneProxy>&	GetDirectionalLightSceneProxies()const;
		RSceneProxyMapping<RStaticMeshSceneProxy>&				GetStaticMeshSceneProxies();
		const RSceneProxyMapping<RStaticMeshSceneProxy>&		GetStaticMeshSceneProxies()const;
		RSceneProxyMapping<RSkeletalMeshSceneProxy>&			GetSkeletalMeshSceneProxies();
		const RSceneProxyMapping<RSkeletalMeshSceneProxy>&		GetSkeletalMeshSceneProxies()const;
	protected:
		void	AddOrRemoveCamera_RenderThread(RViewProxy* InSceneProxy, BOOL32 InIsAdd);
		void	AddOrRemoveSkyLight_RenderThread(RSkyLightSceneProxy* InSceneProxy, BOOL32 InIsAdd);
		void	AddOrRemoveDirectionalLight_RenderThread(RDirectionalLightSceneProxy* InSceneProxy, BOOL32 InIsAdd);
		void	AddOrRemoveStaticMesh_RenderThread(RStaticMeshSceneProxy* InSceneProxy, BOOL32 InIsAdd);
		void	AddOrRemoveSkeletalMesh_RenderThread(RSkeletalMeshSceneProxy* InSceneProxy, BOOL32 InIsAdd);
	protected:
		ROctree		RenderSceneOctree;
	protected:
		RSceneProxyMapping<RViewProxy>					ViewProxies;
		RSceneProxyMapping<RSkyLightSceneProxy>			SkyLightProxies;
		RSceneProxyMapping<RDirectionalLightSceneProxy>	DirectionalLightSceneProxies;
		RSceneProxyMapping<RStaticMeshSceneProxy>		StaticMeshSceneProxies;
		RSceneProxyMapping<RSkeletalMeshSceneProxy>		SkeletalMeshSceneProxies;
	protected:
		RCommand	RenderAddCommands;
		RCommand	RenderRemoveCommands;
		RCommand	RenderUpdateCommands;
	public:
		RScene();
		virtual ~RScene();
		RScene(const RScene&) = delete;
		RScene& operator=(const RScene&) = delete;
	};

	class RSceneNull : public EObjectBase, public RSceneInterface
	{
	public:
		virtual void	ClearAll()override {}
		virtual void	UnbindErrorCheck()override {}
	public:
		virtual void	AddCamera(PCameraComponent* InComponent)override {}
		virtual void	RemoveCamera(PCameraComponent* InComponent)override {}
		virtual void	UpdateCamera(PCameraComponent* InComponent)override {}

		virtual void	AddSkyLight(PSkyLightComponent* InComponent)override {}
		virtual void	RemoveSkyLight(PSkyLightComponent* InComponent)override {}
		virtual void	UpdateSkyLight(PSkyLightComponent* InComponent)override {}

		virtual void	AddDirectionalLight(PDirectionalLightComponent* InComponent)override {}
		virtual void	RemoveDirectionalLight(PDirectionalLightComponent* InComponent)override {}
		virtual void	UpdateDirectionalLight(PDirectionalLightComponent* InComponent)override {}

		virtual void	AddStaticMesh(PStaticMeshComponent* InComponent)override {}
		virtual void	RemoveStaticMesh(PStaticMeshComponent* InComponent)override {}
		virtual void	UpdateStaticMesh(PStaticMeshComponent* InComponent)override {}
		virtual void	AddSkeletalMesh(PSkeletalMeshComponent* InComponent)override {}
		virtual void	RemoveSkeletalMesh(PSkeletalMeshComponent* InComponent)override {}
		virtual void	UpdateSkeletalMesh(PSkeletalMeshComponent* InComponent)override {}
	public:
		RSceneNull() {}
		RSceneNull(const RSceneNull& Other) {}
		virtual ~RSceneNull() {}
		RSceneNull& operator=(const RSceneNull& Other) { return (*this); }
	};

};