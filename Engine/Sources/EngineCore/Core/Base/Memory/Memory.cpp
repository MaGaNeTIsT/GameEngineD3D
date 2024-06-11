#include "Memory.h"

namespace PigeonEngine
{

#if 1

	PE_INLINE void EMemory::Memcpy(void* Dst, void const* Src, UINT32 Size)
	{
		std::memcpy(Dst, Src, Size);
	}
	PE_INLINE void EMemory::Memset(void* Dst, UINT8 Val, UINT32 Size)
	{
		std::memset(Dst, Val, Size);
	}
	PE_INLINE void EMemory::Memmov(void* Dst, void const* Src, UINT32 Size)
	{
		std::memmove(Dst, Src, Size);
	}

#endif

};