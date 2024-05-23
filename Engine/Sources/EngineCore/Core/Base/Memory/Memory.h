#pragma once

#include "../../../Main/Main.h"
#include "../DataStructure/BuiltInType.h"
#include <Config/ErrorCaption.h>
#include "../../../../Development/Alert/DevelopmentDefines.h"
#include "Base/Math/Math.h"
#include <functional>
#include <memory>

namespace PigeonEngine
{

#if 0

	template<typename _TResult, typename... _TArguments>
	using TFunction = std::function<_TResult (_TArguments...)>;
	template<typename _T>
	using TRemoveRef = std::remove_reference_t<_T>;

	template<typename _T>
	using TUniquePtr = std::unique_ptr<_T>;
	template<typename _T>
	using TSharedPtr = std::shared_ptr<_T>;
	template<typename _T>
	using TWeakPtr = std::weak_ptr<_T>;
	template<typename _T>
	using TInitializerList = std::initializer_list<_T>;

	class EMemory final
	{
	public:
		PE_INLINE static void Memcpy(void* Dst, void const* Src, UINT32 Size);
		PE_INLINE static void Memset(void* Dst, UINT8 Val, UINT32 Size);
		PE_INLINE static void Memmov(void* Dst, void const* Src, UINT32 Size);
	public:
		template<typename _T>
		PE_INLINE PE_NODISCARD PE_STATIC_CONSTEXPR TRemoveRef<_T>&& Move(_T&& _In)noexcept
		{
			return std::move<_T>(_In);
		}

		template<typename _T>
		PE_INLINE PE_NODISCARD PE_STATIC_CONSTEXPR _T&& Forward(TRemoveRef<_T>&& _In) noexcept
		{
			return std::forward<_T>(_In);
		}

		template<class _T, class... _TArguments>
		PE_INLINE PE_NODISCARD static TUniquePtr<_T> MakeUnique(_TArguments&&... _InArguments)
		{
			return std::make_unique(_InArguments);
		}

		template<class _T, class... _TArguments>
		PE_INLINE PE_NODISCARD PE_STATIC_CONSTEXPR TUniquePtr<_T> MakeUnique(_TArguments&&... _InArguments)
		{
			return std::make_unique(_InArguments);
		}

		template<class _T, typename _TSizeType = INT32>
		PE_INLINE PE_NODISCARD static TUniquePtr<_T> MakeUnique(_TSizeType _InSize)
		{
			return std::make_unique<_T>(_InSize);
		}

		template<class _T, typename _TSizeType = INT32>
		PE_INLINE PE_NODISCARD PE_STATIC_CONSTEXPR TUniquePtr<_T> MakeUnique(_TSizeType _InSize)
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
		PE_INLINE void Append(const THeapBase& Other)
		{
			if (Other.HeapSize > (_TSizeType)0)
			{
				_TSizeType NewSize = Other.HeapSize + HeapSize;
				TUniquePtr<UINT8[]> NewHeapPtr = EMemory::MakeUnique<UINT8[]>(NewSize);
				UINT8* RawDataPtr = NewHeapPtr.get();
				if (HeapSize > (_TSizeType)0)
				{
					EMemory::Memmov(RawDataPtr, Data(), HeapSize);
					RawDataPtr = &(RawDataPtr[HeapSize]);
				}
				EMemory::Memcpy(RawDataPtr, Other.Data(), Other.HeapSize);
				HeapPtr = EMemory::Move(NewHeapPtr);
				HeapSize = EMemory::Move(NewSize);
			}
		}
		PE_INLINE void Append(THeapBase&& Other)
		{
			if (Other.HeapSize > (_TSizeType)0)
			{
				_TSizeType NewSize = Other.HeapSize + HeapSize;
				TUniquePtr<UINT8[]> NewHeapPtr = EMemory::MakeUnique<UINT8[]>(NewSize);
				UINT8* RawDataPtr = NewHeapPtr.get();
				if (HeapSize > (_TSizeType)0)
				{
					EMemory::Memmov(RawDataPtr, Data(), HeapSize);
					RawDataPtr = &(RawDataPtr[HeapSize]);
				}
				EMemory::Memmov(RawDataPtr, Other.Data(), Other.HeapSize);
				HeapPtr = EMemory::Move(NewHeapPtr);
				HeapSize = EMemory::Move(NewSize);
			}
		}
		PE_INLINE void Append(_TSizeType InExtentSize)
		{
			if (InExtentSize > (_TSizeType)0)
			{
				_TSizeType NewSize = InExtentSize + HeapSize;
				TUniquePtr<UINT8[]> NewHeapPtr = EMemory::MakeUnique<UINT8[]>(NewSize);
				if (HeapSize > (_TSizeType)0)
				{
					UINT8* RawDataPtr = NewHeapPtr.get();
					EMemory::Memmov(RawDataPtr, Data(), HeapSize);
				}
				HeapPtr = EMemory::Move(NewHeapPtr);
				HeapSize = EMemory::Move(NewSize);
			}
		}
		PE_INLINE void Substract(_TSizeType InExtentSize)
		{
			if (InExtentSize > (_TSizeType)0)
			{
				_TSizeType RestSize = HeapSize >= InExtentSize ? (HeapSize - InExtentSize) : (_TSizeType)0;
				TUniquePtr<UINT8[]> NewHeapPtr = nullptr;
				if (RestSize > (_TSizeType)0)
				{
					NewHeapPtr = EMemory::MakeUnique<UINT8[]>(RestSize);
					UINT8* RawDataPtr = NewHeapPtr.get();
					EMemory::Memmov(RawDataPtr, Data(), RestSize);
				}
				HeapPtr = NewHeapPtr ? EMemory::Move(NewHeapPtr) : nullptr;
				HeapSize = EMemory::Move(RestSize);
			}
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
		PE_INLINE void* Data()
		{
			return ((void*)(HeapPtr.get()));
		}
		PE_INLINE const void* Data()const
		{
			return ((const void*)(HeapPtr.get()));
		}
		PE_INLINE _TSizeType Size()const
		{
			return HeapSize;
		}
	public:
		~THeapBase()
		{
			Release();
		}
		PE_INLINE THeapBase()noexcept
			: HeapPtr(nullptr), HeapSize((_TSizeType)0)
		{
		}
		PE_INLINE THeapBase(_TSizeType InInitSize)noexcept
			: HeapPtr(nullptr), HeapSize((_TSizeType)0)
		{
			Allocate(InInitSize);
		}
		PE_INLINE THeapBase(const THeapBase& Other)noexcept
			: HeapPtr(nullptr), HeapSize((_TSizeType)0)
		{
			Copy(Other);
		}
		PE_INLINE THeapBase(THeapBase&& Other)noexcept
			: HeapPtr(nullptr), HeapSize((_TSizeType)0)
		{
			HeapPtr = EMemory::Move(Other.HeapPtr);
			HeapSize = EMemory::Move(Other.HeapSize);
		}
		PE_INLINE THeapBase& operator=(const THeapBase& Other)noexcept
		{
			Release();
			if (Other.HeapSize > (_TSizeType)0)
			{
				Allocate(Other.HeapSize);
				Copy(Other);
			}
		}
		PE_INLINE THeapBase& operator=(THeapBase&& Other)noexcept
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

#endif

};