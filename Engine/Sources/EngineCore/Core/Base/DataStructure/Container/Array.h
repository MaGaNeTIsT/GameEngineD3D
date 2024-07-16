#pragma once

#include "../../../../Main/Main.h"
#include <Base/DataStructure/BuiltInType.h>
#include "../../../../Core/Config/ErrorCaption.h"
#include <vector>
#include <algorithm>
#include <random>
#include <rapidjson.h>
#include <array>
#include "../../../../../Development/Alert/DevelopmentDefines.h"

#include "Base/Memory/Memory.h"

namespace PigeonEngine
{

#if 0

    template <typename _Ty, typename _TSizeType = INT32, typename _TAllocator = TDefaultAllocator<_Ty, _TSizeType>>
    class TArray final
    {
    public:
        _TSizeType Add(const _Ty& InElement)
        {
            PushInternal(InElement);
            Check((ElementNum > (_TSizeType)0));
            return (ElementNum - (_TSizeType)1);
        }
        _TSizeType Add(_Ty&& InElement)
        {
            PushInternal(EMemory::Forward<_Ty>(InElement));
            Check((ElementNum > (_TSizeType)0));
            return (ElementNum - (_TSizeType)1);
        }
        _Ty& operator[](_TSizeType InIndex)
        {
            Check(((InIndex >= (_TSizeType)0) && (InIndex < ElementNum)));
            return (Allocator.HeapData()[InIndex]);
        }
        const _Ty& operator[](_TSizeType InIndex)const
        {
            Check(((InIndex >= (_TSizeType)0) && (InIndex < ElementNum)));
            return (Allocator.HeapData()[InIndex]);
        }
        _Ty& AddDefaultGetRef()
        {
            return PushDefaultInternal();
        }
        _Ty& GetRef(_TSizeType InIndex)
        {
            Check(((InIndex >= (_TSizeType)0) && (InIndex < ElementNum)));
            return (Allocator.HeapData()[InIndex]);
        }
        const _Ty& Get(_TSizeType InIndex)const
        {
            Check(((InIndex >= (_TSizeType)0) && (InIndex < ElementNum)));
            return (Allocator.HeapData()[InIndex]);
        }
        _Ty& First()
        {
        }
        const _Ty& First()const
        {

        }
        _Ty& Last()
        {

        }
        const _Ty& Last()const
        {

        }
        _TSizeType Find(const _Ty& InElement) const
        {

        }
        BOOL32 Contains(const T& Element) const;
        void Resize(const UINT32& NewSize);
        void Recapacity(const UINT32& NewCapacity);
        void RemoveAt(const UINT32& Index);
        void Remove(const T& Element);
        void Pop();
        void Clear();

        T* RawData();
        const T* RawData() const;

        void Append(const TArray<T>& Other);

        void       Sort();
        void       Sort(BOOL32 Predicate(const T&, const T&));
        void       Shuffle();
        TArray<T>  Reverse() const;

    public:
        ~TArray()
        {
            EmptyInternal();
        }
        TArray()noexcept
            : ElementNum((_TSizeType)0)
        {
        }
        TArray(_TSizeType InElementNum)noexcept
            : ElementNum((_TSizeType)0)
        {
            AppendElementInternal(InElementNum);
        }
        TArray(TInitializerList<_Ty> InInitList)noexcept
            : ElementNum((_TSizeType)0)
        {
            const _TSizeType NumList = InitializerListSize<_Ty, _TSizeType>(InInitList);
            if (NumList > (_TSizeType)0)
            {
                AppendCapacityInternal(NumList);
                const _Ty* ListBegin = InitializerListBegin<_Ty>(InInitList);
                for (_TSizeType i = (_TSizeType)0; i < NumList; i++)
                {
                    PushInternal(ListBegin[i]);
                }
            }
        }
        TArray(const TArray& Other)noexcept
            : ElementNum((_TSizeType)0)
        {
            CopyFromInternal(Other);
        }
        TArray(TArray&& Other)noexcept
            : ElementNum((_TSizeType)0)
        {
            MoveFromInternal(EMemory::Forward<TArray>(Other));
        }
        TArray& operator=(const TArray& Other)noexcept
        {
            CopyFrom(Other);
            return (*this);
        }
        TArray& operator=(TArray&& Other)noexcept
        {
            MoveFrom(EMemory::Forward<TArray>(Other));
            return (*this);
        }
    public:
        void CopyFrom(const TArray& Other)
        {
            EmptyInternal();
            CopyFromInternal(Other);
        }
        void MoveFrom(TArray&& Other)
        {
            EmptyInternal();
            MoveFromInternal(EMemory::Forward<TArray>(Other));
        }
        _TSizeType Num() const { return ElementNum; }
        _TSizeType AllocateSize() const { return (Allocator.HeapSize()); }
        _TSizeType Capacity() const
        {
            PE_CONSTEXPR _TSizeType TypeSize = (_TSizeType)(sizeof(_Ty));
            return (Allocator.HeapSize() / TypeSize);
        }
    private:
        void CopyFromInternal(const TArray& Other)
        {
            const _TSizeType NumOther = Other.Num();
            if (NumOther > (_TSizeType)0)
            {
                AppendCapacityInternal(NumOther);
                for (_TSizeType i = (_TSizeType)0; i < NumList; i++)
                {
                    PushInternal(Other[i]);
                }
            }
        }
        void MoveFromInternal(TArray&& Other)
        {
            const _TSizeType NumOther = Other.Num();
            if (NumOther > (_TSizeType)0)
            {
                AppendCapacityInternal(NumOther);
                for (_TSizeType i = (_TSizeType)0; i < NumList; i++)
                {
                    PushInternal(EMemory::Move<_Ty>(Other[i]));
                }
            }
        }
        void AppendCapacityInternal(_TSizeType InElementNum)
        {
            PE_CONSTEXPR _TSizeType TypeSize = (_TSizeType)(sizeof(_Ty));
            if (InElementNum == (_TSizeType)0)
            {
                return;
            }
            _TSizeType NewAppendSize = InElementNum * TypeSize;
            Allocator.Append(NewAppendSize);
        }
        void AppendElementInternal(_TSizeType InElementNum)
        {
            PE_CONSTEXPR _TSizeType TypeSize = (_TSizeType)(sizeof(_Ty));
            if (InElementNum == (_TSizeType)0)
            {
                return;
            }
            AppendCapacityInternal(InElementNum);
            for (_TSizeType i = (_TSizeType)0; i < InElementNum; i++)
            {
                _Ty* NewItem = new (&(Allocator.HeapData()[ElementNum + i]))_Ty();
            }
            ElementNum = ElementNum + InElementNum;
        }
        void EmptyInternal(_TSizeType InElementNum = (_TSizeType)0)
        {
            PE_CONSTEXPR _TSizeType TypeSize = (_TSizeType)(sizeof(_Ty));
            for (_TSizeType i = (_TSizeType)0; i < ElementNum; i++)
            {
                (Allocator.HeapData()[i]).~_Ty();
            }
            ElementNum = (_TSizeType)0;
            if (InElementNum > (_TSizeType)0)
            {
                Allocator.Reallocate(InElementNum * TypeSize);
            }
        }
        _Ty& PushDefaultInternal()
        {
            PE_CONSTEXPR _TSizeType TypeSize = (_TSizeType)(sizeof(_Ty));
            if (ElementNum >= Capacity())
            {
                Allocator.Growth();
            }
            _Ty* NewItem = new (&(Allocator.HeapData()[ElementNum]))_Ty();
            ElementNum = ElementNum + (_TSizeType)1;
            return (*NewItem);
        }
        void PushInternal(const _Ty& InItem)
        {
            PE_CONSTEXPR _TSizeType TypeSize = (_TSizeType)(sizeof(_Ty));
            if (ElementNum >= Capacity())
            {
                Allocator.Growth();
            }
            _Ty* NewItem = new (&(Allocator.HeapData()[ElementNum]))_Ty(InItem);
            ElementNum = ElementNum + (_TSizeType)1;
        }
        void PushInternal(_Ty&& InItem)
        {
            PE_CONSTEXPR _TSizeType TypeSize = (_TSizeType)(sizeof(_Ty));
            if (ElementNum >= Capacity())
            {
                Allocator.Growth();
            }
            _Ty* NewItem = new (&(Allocator.HeapData()[ElementNum]))_Ty(EMemory::Forward<_Ty>(InItem));
            ElementNum = ElementNum + (_TSizeType)1;
        }
        void PopInternal()
        {
            if (ElementNum == (_TSizeType)0)
            {
                return;
            }
            (Allocator.HeapData()[ElementNum - (_TSizeType)1]).~_Ty();
            ElementNum = ElementNum - (_TSizeType)1;
        }
        void ShrinkInternal()
        {
            PE_CONSTEXPR _TSizeType TypeSize = (_TSizeType)(sizeof(_Ty));
            const _TSizeType RestNum = Capacity() - ElementNum;
            _TSizeType SubNum = ((RestNum / Allocator.GetAllocateStepSize()) > (_TSizeType)0) ? Allocator.GetAllocateStepSize() : (RestNum % Allocator.GetAllocateStepSize());
            SubNum = RestNum - SubNum;
            if (SubNum > (_TSizeType)0)
            {
                Allocator.Substract(TypeSize * SubNum);
            }
        }
    private:
        _TSizeType      ElementNum;
        _TAllocator     Allocator;
    };

#else

    // -------------------   Declarations   -----------------------------
    template <typename T>
    class TArray
    {
    public:
        using TIterator         = typename std::vector<T>::iterator;
        using TConstIterator    = typename std::vector<T>::const_iterator;
        using SizeType = UINT32;
    public:
        TArray();
        TArray(const TArray<T>& Other);

        explicit TArray(const std::vector<T>& Other);
        TArray(TArray<T>&& Other) noexcept;
        explicit TArray(std::vector<T>&& Other);
        
        ~TArray() = default;
        
        T&          operator[](const UINT32& Index);
        const T&    operator[](const UINT32& Index)const;
        TArray<T>&  operator= (const TArray<T>& Other);
        TArray<T>&  operator= (TArray<T>&& Other) noexcept;
        TArray<T>&  operator= (std::vector<T>&& Other);
        
        UINT32        Add       (const T& Element);
        UINT32        Add       (T&& Element);

        T&          Add_Default_GetRef()
        {
            T Temp;
            Elements.push_back(std::move(Temp));
            return Elements[Elements.size() - 1u];
        }
        
        T&          GetRef    (const UINT32& Index);
        const T&    Get       (const UINT32& Index)const;
        const T&    Last      ();
        // Find the index of given Element, return array's Length if the Element doesn't exist.
        UINT32 Find      (const T& Element) const;
        BOOL32 Contains  (const T& Element) const;
        void Resize    (const UINT32& NewSize);
        void Recapacity(const UINT32& NewCapacity);
        void RemoveAt  (const UINT32& Index);
        void Remove    (const T& Element);
        void Pop       ();
        void Clear     ();

        T*          RawData();
        const T*    RawData() const;

        void Append(const TArray<T>& Other);

        void       Sort   ();
        void       Sort   (BOOL32 Predicate(const T&, const T&));
        void       Shuffle();
        TArray<T>  Reverse() const;
        
        typename TIterator Begin()
        {
            return Elements.begin();
        }
        typename TConstIterator Begin()const
        {
            return Elements.begin();
        }
        typename TIterator End()
        {
            return Elements.end();
        }
        typename TConstIterator End()const
        {
            return Elements.end();
        }
        typename TIterator Rbegin()
        {
            return Elements.rbegin();
        }
        typename TIterator Rend()
        {
            return Elements.rend();
        }

        /**
         * DO NOT USE DIRECTLY
         * STL-like iterators to enable range-based for loop support.
         */
        typename TConstIterator begin()const{return Elements.begin();}
        typename TConstIterator end()const{return Elements.end();}
        typename TIterator      begin(){return Elements.begin();}
        typename TIterator      end(){return Elements.end();}
        
        PE_NODISCARD UINT32 Length()const
        {
            return static_cast<UINT32>(this->Elements.size());
        }
        PE_NODISCARD UINT32 LastIndex    ()const;

    
    private:
        class std::vector<T> Elements;
    };


    // -------------------   Implementations   -----------------------------
    template <typename T>
    TArray<T>::TArray()
    {
    }

    template <typename T>
    TArray<T>::TArray(const TArray<T>& Other)
        :
    Elements(Other.Elements)
    {
        
    }

    template <typename T>
    TArray<T>::TArray(const std::vector<T>& Other)
        :
    Elements(Other)
    {
    }

    template <typename T>
    TArray<T>::TArray(TArray<T>&& Other) noexcept
        :
    Elements(std::move(Other.Elements))
    {
        Other.Clear();
    }

    template <typename T>
    TArray<T>::TArray(std::vector<T>&& Other)
        :
    Elements(std::move(Other))
    {
        Other.clear();
    }

    template <typename T>
    T& TArray<T>::operator[](const UINT32& Index)
    {
#if _EDITOR_ONLY
        UINT32 NumElements = Length();
        PE_CHECK(ENGINE_ARRAY_ERROR, "Array has no this index", (Index < NumElements));
#endif
        return Elements[Index];
    }

    template <typename T>
    const T& TArray<T>::operator[](const UINT32& Index)const
    {
#if _EDITOR_ONLY
        UINT32 NumElements = Length();
        PE_CHECK(ENGINE_ARRAY_ERROR, "Array has no this index", (Index < NumElements));
#endif
        return Elements[Index];
    }

    template <typename T>
    TArray<T>& TArray<T>::operator=(const TArray<T>& Other)
    {
         Elements = Other.Elements;
         return *this;
    }

    template <typename T>
    TArray<T>& TArray<T>::operator=(TArray<T>&& Other) noexcept
    {
        Elements = std::move(Other.Elements);
        Other.Clear();
        return *this;
    }

    template <typename T>
    TArray<T>& TArray<T>::operator=(std::vector<T>&& Other)
    {
         Elements = std::move(Other);
         Other.clear();
         return *this;
    }
    
    template <typename T>
    UINT32 TArray<T>::LastIndex() const
    {
        return Length() > 0 ? Length() - 1 : 0;
    }

    template <typename T>
    UINT32 TArray<T>::Add(const T& Element)
    {
         this->Elements.push_back(Element);
         return LastIndex();
    }

    template <typename T>
    UINT32 TArray<T>::Add(T&& Element)
    {
        this->Elements.push_back(Element);
        return LastIndex();
    }

    template <typename T>
    T& TArray<T>::GetRef(const UINT32& Index)
    {
#if _EDITOR_ONLY
        UINT32 NumElements = Length();
        PE_CHECK(ENGINE_ARRAY_ERROR, "Array has no this index", (Index < NumElements));
#endif
        return this->Elements[Index];
    }

    template <typename T>
    const T& TArray<T>::Get(const UINT32& Index)const
    {
#if _EDITOR_ONLY
        UINT32 NumElements = Length();
        PE_CHECK(ENGINE_ARRAY_ERROR, "Array has no this index", (Index < NumElements));
#endif
        return this->Elements[Index];
    }

    template <typename T>
    const T& TArray<T>::Last()
    {
        return Elements[LastIndex()];
    }

    template <typename T>
    void TArray<T>::RemoveAt(const UINT32& Index)
    {
        if(Index >= Length())
        {
            return;
        }
        this->Elements.erase(Elements.begin() + Index);
    }

    template <typename T>
    void TArray<T>::Remove(const T& Element)
    {
        const UINT32& i = Find(Element);
        if(i != Length())
        {
            RemoveAt(i);
        }
    }

    template <typename T>
    void TArray<T>::Pop()
    {
        Elements.pop_back();
    }

    template <typename T>
    void TArray<T>::Clear()
    {
        Elements.clear();
    }

    template <typename T>
    T* TArray<T>::RawData()
    {
        return Elements.data();
    }

    template <typename T>
    const T* TArray<T>::RawData() const
    {
        return Elements.data();
    }

    template <typename T>
    void TArray<T>::Append(const TArray<T>& Other)
    {
         const UINT32 Size = Length() + Other.Length();
         Recapacity(Size * 2);
         for(const auto& elem:Other)
         {
             Add(elem);
         }
    }

    template <typename T>
    void TArray<T>::Sort()
    {
        std::sort(Elements.begin(), Elements.end());
    }

    template <typename T>
    void TArray<T>::Sort(BOOL32 Predicate(const T&, const T&))
    {
        std::sort(Elements.begin(), Elements.end(),Predicate);
    }
    
    template <typename T>
    void TArray<T>::Shuffle()
    {
        std::random_device rd;
        std::mt19937 rng(rd());
        std::shuffle(Elements.begin(), Elements.end(),rng);
    }

    template <typename T>
    TArray<T> TArray<T>::Reverse() const
    {
        std::vector<T> Temp = Elements;
        std::reverse(Temp.begin(), Temp.end());
        return TArray<T>(Temp);
    }

    template <typename T>
    UINT32 TArray<T>::Find(const T& Element) const
    {
        for(UINT32 i = 0; i < Elements.size();++i)
        {
            if(Elements[i] == Element)
            {
                return i;
            }
        }
        return Length();
    }

    template <typename T>
    BOOL32 TArray<T>::Contains(const T& Element) const
    {
         for(const auto& elem : Elements)
         {
             if(elem == Element)
             {
                 return TRUE;
             }
         }
         return FALSE;
    }

    template <typename T>
    void TArray<T>::Resize(const UINT32& NewSize)
    {
        if ((NewSize > 0u) && (this->Elements.size() != NewSize))
        {
            this->Elements.resize(NewSize);
        }
    }

    template <typename T>
    void TArray<T>::Recapacity(const UINT32& NewCapacity)
    {
         if ((NewCapacity > 0u) && (this->Elements.capacity() != NewCapacity))
         {
             this->Elements.reserve(NewCapacity);
         }
    }

#endif

};
