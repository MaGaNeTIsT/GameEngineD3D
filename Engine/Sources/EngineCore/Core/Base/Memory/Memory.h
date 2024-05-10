#pragma once

#include "../../../Main/Main.h"
#include "../DataStructure/BuiltInType.h"
#include <Config/ErrorCaption.h>
#include "../../../../Development/Alert/DevelopmentDefines.h"
#include <memory>

namespace PigeonEngine
{

	class EMemory final
	{
	public:
		PE_INLINE static void Memcpy(void* Dst, void const* Src, UINT32 Size);
		PE_INLINE static void Memset(void* Dst, UINT8 Val, UINT32 Size);
		PE_INLINE static void Memmov(void* Dst, void const* Src, UINT32 Size);
	public:
		template<typename _T>
		PE_INLINE PE_STATIC_CONSTEXPR std::remove_reference_t<_T>&& Move(_T&& _In)noexcept
		{
			return std::move<_T>(_In);
		}
		template<typename _T>
		PE_INLINE PE_STATIC_CONSTEXPR _T&& Forward(std::remove_reference_t<_T>&& _In) noexcept
		{
			return std::forward<_T>(_In);
		}
	private:
		EMemory()noexcept {}
		EMemory(const EMemory&)noexcept {}
		EMemory(EMemory&)noexcept {}
		~EMemory() {}
		EMemory& operator=(const EMemory&)noexcept { return (*this); }
		EMemory& operator=(EMemory&&)noexcept { return (*this); }
	public:
		static EMemory* GetSingleton()
		{
			static EMemory _MemorySingletonObject;
			return (&_MemorySingletonObject);
		}
	};

	template<typename _T, typename _TSizeType = INT32>
	class THeapBase final
	{
	public:
		THeapBase()noexcept
		{

		}
		THeapBase(_TSizeType InInitSize)noexcept
		{

		}
		~THeapBase()
		{

		}
		THeapBase(const THeapBase& Other)noexcept
		{

		}
		THeapBase(THeapBase&& Other)noexcept
		{

		}
		THeapBase& operator=(const THeapBase& Other)noexcept
		{

		}
		THeapBase& operator=(THeapBase&& Other)noexcept
		{

		}
	private:

	};

};