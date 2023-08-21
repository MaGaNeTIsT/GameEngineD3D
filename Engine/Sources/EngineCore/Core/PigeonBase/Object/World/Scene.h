#pragma once

#include <CoreMinimal.h>

#include "Base/DataStructure/Container/Array.h"
#include "PigeonBase/Object/Object.h"


namespace PigeonEngine
{
	class PWorld;
	class PActor;



	/*
	 * used for serialization and deserializaition to a scene.
	 */
	class PScene : public PObject
	{
	public:
		
		CLASS_VIRTUAL_NOCOPY_BODY(PScene)
		
	protected:
		


	};

};