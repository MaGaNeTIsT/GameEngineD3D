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

#if 1

#define SMART_PTR_GET(__PtrType) \
	template<typename _Ty>\
	static PE_INLINE PE_NODISCARD _Ty* GetPtr(__PtrType<_Ty>&& _In)\
	{\
		return (_In.get());\
	}\
	template<typename _Ty>\
	static PE_INLINE PE_NODISCARD _Ty* GetPtr(__PtrType<_Ty>& _In)\
	{\
		return (_In.get());\
	}\
	template<typename _Ty>\
	static PE_INLINE PE_NODISCARD const _Ty* GetPtr(const __PtrType<_Ty>& _In)\
	{\
		return (_In.get());\
	}\


	//template<typename _TResult, typename... _TArguments>
	//using TFunction = std::function<_TResult(_TArguments...)>;

	template<typename _Ty>
	using TRemoveRef = std::remove_reference_t<_Ty>;

	template<typename _Ty>
	using TUniquePtr = std::unique_ptr<_Ty>;
	template<typename _Ty>
	using TSharedPtr = std::shared_ptr<_Ty>;
	template<typename _Ty>
	using TWeakPtr = std::weak_ptr<_Ty>;
	template<typename _Ty>
	using TInitializerList = std::initializer_list<_Ty>;

	class EMemory final
	{
	public:
		static PE_INLINE void Memcpy(void* Dst, void const* Src, UINT32 Size);
		static PE_INLINE void Memset(void* Dst, UINT8 Val, UINT32 Size);
		static PE_INLINE void Memmov(void* Dst, void const* Src, UINT32 Size);
	public:
		template <typename _Ty>
		static PE_INLINE PE_NODISCARD constexpr TRemoveRef<_Ty>&& Move(_Ty&& _In)noexcept
		{
			return std::move<_Ty>(_In);
		}

		template <typename _Ty>
		static PE_INLINE PE_NODISCARD constexpr _Ty&& Forward(TRemoveRef<_Ty>& _In)noexcept
		{
			return std::forward<_Ty>(_In);
		}

		template <typename _Ty>
		static PE_INLINE PE_NODISCARD constexpr _Ty&& Forward(TRemoveRef<_Ty>&& _In)noexcept
		{
			return std::forward<_Ty>(_In);
		}

		template <typename _Ty, typename... _TArguments>
		static PE_NODISCARD PE_INLINE TUniquePtr<_Ty> MakeUnique(_TArguments&&... _InArgs)
		{
			return std::make_unique<_Ty, _TArguments...>(std::forward<_TArguments>(_InArgs)...);
		}

		template <typename _Ty, typename _TSizeType = INT32>
		static PE_NODISCARD PE_INLINE TUniquePtr<_Ty> MakeUnique(const _TSizeType _InSize)
		{
			return std::make_unique<_Ty>(_InSize);
		}

		template <typename _Ty, typename... _TArguments>
		static PE_INLINE PE_NODISCARD TSharedPtr<_Ty> MakeShared(_TArguments&&... _InArgs)
		{
			return std::make_shared<_Ty, _TArguments...>(std::forward<_TArguments>(_InArgs)...);
		}

		SMART_PTR_GET(TUniquePtr)
		SMART_PTR_GET(TSharedPtr)
		SMART_PTR_GET(TWeakPtr)
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

	template<typename _TSizeType = INT32, _TSizeType _AllocateSize = (_TSizeType)8>
	class THeapBase final
	{
	public:
		PE_INLINE void Growth()
		{
			Append(_AllocateSize);
		}
		PE_INLINE void Reallocate(_TSizeType InSize)
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
					Reallocate(Other.HeapSize);
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
			Reallocate(_AllocateSize);
		}
		PE_INLINE THeapBase(_TSizeType InInitSize)noexcept
			: HeapPtr(nullptr), HeapSize((_TSizeType)0)
		{
			Reallocate(InInitSize);
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
				Reallocate(Other.HeapSize);
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

	template<typename _Ty, typename _TSizeType, _TSizeType _AllocateSize>
	class TAllocatorBase
	{
	public:
		using THeapType = THeapBase<_TSizeType, sizeof(_Ty)* _AllocateSize>;
	public:
		PE_INLINE void Growth()
		{
			Heap.Growth();
		}
		PE_INLINE void Reallocate(_TSizeType InSize)
		{
			Heap.Reallocate(InSize);
		}
		PE_INLINE void Release()
		{
			Heap.Release();
		}
		PE_INLINE void Append(const TAllocatorBase& Other)
		{
			Heap.Append(Other.Heap);
		}
		PE_INLINE void Append(TAllocatorBase&& Other)
		{
			Heap.Append(EMemory::Forward<THeapType>(Other.Heap));
		}
		PE_INLINE void Append(_TSizeType InExtentSize)
		{
			Heap.Append(InExtentSize);
		}
		PE_INLINE void Substract(_TSizeType InExtentSize)
		{
			Heap.Substract(InExtentSize);
		}
		PE_INLINE void CopyFrom(const TAllocatorBase& Other)
		{
			Heap.Copy(Other.Heap);
		}
		PE_INLINE void MoveFrom(TAllocatorBase&& Other)
		{
			Release();
			if (Other.HeapSize() > (_TSizeType)0)
			{
				Heap = EMemory::Move(Other.Heap);
			}
		}
		PE_INLINE _Ty* HeapData()
		{
			return ((_Ty*)(Heap.Data()));
		}
		PE_INLINE const _Ty* HeapData()const
		{
			return ((const _Ty*)(Heap.Data()));
		}
		PE_INLINE _TSizeType HeapSize()const
		{
			return Heap.Size();
		}
	public:
		virtual ~TAllocatorBase()
		{
			Release();
		}
		PE_INLINE TAllocatorBase()noexcept
		{
		}
		PE_INLINE TAllocatorBase(_TSizeType InInitSize)noexcept
			: Heap(InInitSize)
		{
		}
		PE_INLINE TAllocatorBase(const TAllocatorBase& Other)noexcept = delete;
		PE_INLINE TAllocatorBase(TAllocatorBase&& Other)noexcept = delete;
		PE_INLINE TAllocatorBase& operator=(const TAllocatorBase& Other)noexcept = delete;
		PE_INLINE TAllocatorBase& operator=(TAllocatorBase&& Other)noexcept = delete;
	private:
		THeapType Heap;
	};

	template<typename _Ty, typename _TSizeType = INT32, _TSizeType _DefaultAllocateSize = (_TSizeType)8>
	using TDefaultAllocator = TAllocatorBase<_Ty, _TSizeType, _DefaultAllocateSize>;

#endif

};