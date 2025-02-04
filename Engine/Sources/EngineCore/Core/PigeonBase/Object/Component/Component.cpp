#include "./Component.h"

namespace PigeonEngine
{

	static void RegisterClassTypes()
	{
		RegisterClassType<PComponent, PObject>();
	}

	PE_REGISTER_CLASS_TYPE(&RegisterClassTypes);

	PComponent::PComponent()
	{
		POBJ_DEBUGNAME_SET(this, "Component");
	}
	PComponent::~PComponent()
	{
	}

	void PComponent::Destroy()
	{
		PObject::Destroy();
	}


	void PComponent::Init()
	{
	}

	void PComponent::Uninit()
	{
		PObject::Uninit();
		
	}

	void PComponent::Tick(FLOAT deltaTime)
	{
#if _EDITOR_ONLY
		EditorTick(deltaTime);
#else
		FixedTick(deltaTime);
#endif
	}
	void PComponent::FixedTick(FLOAT deltaTime)
	{
		if(!IsTickable())
		{
			return;
		}
	}
#if _EDITOR_ONLY
	void PComponent::EditorTick(FLOAT deltaTime)
	{
		if(!IsTickable())
		{
			return;
		}
	}
#endif

};