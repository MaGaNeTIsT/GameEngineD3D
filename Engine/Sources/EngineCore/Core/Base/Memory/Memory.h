#pragma once

#include "../../../Main/Main.h"
#include "../DataStructure/BuiltInType.h"
#include <Config/ErrorCaption.h>
#include "../../../../Development/Alert/DevelopmentDefines.h"
#include <memory>

namespace PigeonEngine
{

	template<typename _TResult, typename... _TArguments>
	using TFunction = std::function<_TResult(_TArguments...)>;

	template<typename _T>
	using TUniquePtr = std::unique_ptr<_T>;
	template<typename _T>
	using TSharedPtr = std::shared_ptr<_T>;
	template<typename _T>
	using TWeakPtr = std::weak_ptr<_T>;

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

		template<class _T, class... _TArguments>
		PE_INLINE static TUniquePtr<_T> MakeUnique(_TArguments&&... _InArguments)
		{
			return std::make_unique(_InArguments);
		}

		template<class _T, class... _TArguments>
		PE_INLINE PE_STATIC_CONSTEXPR TUniquePtr<_T> MakeUnique(_TArguments&&... _InArguments)
		{
			return std::make_unique(_InArguments);
		}

		template<class _T, typename _TSizeType = INT32>
		PE_INLINE static TUniquePtr<_T> MakeUnique(_TSizeType _InSize)
		{
			return std::make_unique<_T>(_InSize);
		}

		template<class _T, typename _TSizeType = INT32>
		PE_INLINE PE_STATIC_CONSTEXPR TUniquePtr<_T> MakeUnique(_TSizeType _InSize)
		{
			return std::make_unique<_T>(_InSize);
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

	template<typename _TSizeType = INT32>
	class THeapBase final
	{
	public:
		PE_INLINE void Allocate(_TSizeType InSize)
		{
			if (HeapSize == InSize)
			{
				return;
			}
			Release();
			HeapSize = InSize;
			if (HeapSize > (_TSizeType)0)
			{
				HeapPtr = EMemory::MakeUnique<UINT8[]>(HeapSize);
			}
		}
		PE_INLINE void Release()
		{
			HeapPtr = nullptr;
			HeapSize = (_TSizeType)0;
		}
		PE_INLINE void Copy(const THeapBase& Other)
		{
			if ((Other.HeapSize > (_TSizeType)0))
			{
				if (HeapSize < Other.HeapSize)
				{
					Allocate(Other.HeapSize);
				}
				EMemory::Memcpy(Data(), Other.Data(), Other.HeapSize);
			}
		}
		void* Data()
		{
			return ((void*)(HeapPtr.get()));
		}
		const void* Data()const
		{
			return ((const void*)(HeapPtr.get()));
		}
		_TSizeType Size()const
		{
			return HeapSize;
		}
	public:
		THeapBase()noexcept
			: HeapPtr(nullptr), HeapSize((_TSizeType)0)
		{
		}
		THeapBase(_TSizeType InInitSize)noexcept
			: HeapPtr(nullptr), HeapSize((_TSizeType)0)
		{
			Allocate(InInitSize);
		}
		~THeapBase()
		{
			Release();
		}
		THeapBase(const THeapBase& Other)noexcept
			: HeapPtr(nullptr), HeapSize((_TSizeType)0)
		{
			Copy(Other);
		}
		THeapBase(THeapBase&& Other)noexcept
			: HeapPtr(nullptr), HeapSize((_TSizeType)0)
		{
			HeapPtr = EMemory::Move(Other.HeapPtr);
			HeapSize = EMemory::Move(Other.HeapSize);
		}
		THeapBase& operator=(const THeapBase& Other)noexcept
		{
			Release();
			if (Other.HeapSize > (_TSizeType)0)
			{
				Allocate(Other.HeapSize);
				Copy(Other);
			}
		}
		THeapBase& operator=(THeapBase&& Other)noexcept
		{
			Release();
			if (Other.HeapSize > (_TSizeType)0)
			{
				HeapPtr = EMemory::Move(Other.HeapPtr);
				HeapSize = EMemory::Move(Other.HeapSize);
			}
		}
	private:
		TUniquePtr<UINT8[]>	HeapPtr;
		_TSizeType			HeapSize;
	};

};