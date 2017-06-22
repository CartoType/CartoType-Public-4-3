/*
CARTOTYPE_STACK_ALLOCATOR.H
Copyright (C) 2013-2017 CartoType Ltd.
See www.cartotype.com for more information.
*/

#ifndef CARTOTYPE_STACK_ALLOCATOR_H__
#define CARTOTYPE_STACK_ALLOCATOR_H__

#include <cartotype_types.h>

namespace CartoType
{

/**
A fast memory allocator using a stack. Memory is freed by resetting
the entire stack rather than freeing individual items.
*/
class CStackAllocator
    {
    public:
    ~CStackAllocator()
        {
        Clear();
        }
    
    void Clear()
        {
        TBlock* p = iBlockList;
        while (p)
            {
            TBlock* next = p->iNext;
            delete [] (uint8*)p;
            p = next;
            }
        iBlockList = nullptr;
        iStackEnd = iStackTop = nullptr;
        }

    uint8* Alloc(size_t aBytes)
        {
        aBytes = (aBytes + 7) & ~7;
        if ((size_t)(iStackEnd - iStackTop) < aBytes)
            {
            size_t new_block_size = aBytes;
            if (new_block_size < KMinBlockSize)
                new_block_size = KMinBlockSize;
            TBlock* new_block = (TBlock*)(new uint8[sizeof(TBlock) + new_block_size - 8]);
            if (!new_block)
                return nullptr;
            new_block->iNext = iBlockList;
            iBlockList = new_block;
            iStackTop = new_block->iData;
            iStackEnd = iStackTop + new_block_size;
            }
        uint8* p = iStackTop;
        iStackTop += aBytes;
        return p;
        }

    private:
    enum
        {
        KMinBlockSize = 4 * 1024 * 1024
        };

    class TBlock
        {
        public:
        TBlock* iNext = nullptr;
        uint8 iData[8];
        };       
    
    TBlock* iBlockList = nullptr;
    uint8* iStackEnd = nullptr;
    uint8* iStackTop = nullptr;
    };

/** An allocator based on CStackAllocator, to be used with STL containers. */
template<typename T> struct TStlStackAllocator
    {
    template<typename U> friend struct TStlStackAllocator;

    using value_type = T;
    using pointer = T*;

    using propagate_on_container_copy_assignment = std::true_type;
    using propagate_on_container_move_assignment = std::true_type;
    using propagate_on_container_swap = std::true_type;

    explicit TStlStackAllocator(CStackAllocator& a): m_alloc(a) { }

    template <typename U> TStlStackAllocator(TStlStackAllocator<U> const& aOther): m_alloc(aOther.m_alloc) { }

    pointer allocate(std::size_t n)
        {
        return (pointer)(m_alloc.Alloc(n * sizeof(T)));
        }

    void deallocate(pointer,std::size_t)
        {
        }

    template <typename U> bool operator==(TStlStackAllocator<U> const& aOther) const
        {
        return &m_alloc == &aOther.m_alloc;
        }

    template <typename U> bool operator!=(TStlStackAllocator<U> const& aOther) const
        {
        return &m_alloc != &aOther.m_alloc;
        }

    private:
    CStackAllocator& m_alloc;
    };

}

/** An overloaded allocator which uses a stack allocator. */
inline void* operator new(size_t aSize,CartoType::CStackAllocator& aStackAllocator)
    {
    return aStackAllocator.Alloc(aSize);
    }

/** An overloaded array allocator which uses a stack allocator. */
inline void* operator new[](size_t aSize,CartoType::CStackAllocator& aStackAllocator)
    {
    return aStackAllocator.Alloc(aSize);
    }

/** An overloaded delete operator using a stack allocator. */
inline void operator delete(void* /*aPtr*/,CartoType::CStackAllocator& /*aStackAllocator*/)
    {
    }

/** An overloaded array delete operator using a stack allocator. */
inline void operator delete[](void* /*aPtr*/,CartoType::CStackAllocator& /*aStackAllocator*/)
    {
    }

#endif
