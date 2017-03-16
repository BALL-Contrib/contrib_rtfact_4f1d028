/* 
 *  AllocationUtils.hpp
 *
 *  RTfact - Real-Time Ray Tracing Library
 *
 *  Copyright (C) 2009  Saarland University
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  Created on: 2008-11-02 21:38:37 +0100
 *  Author(s): Iliyan Georgiev, Georgiev Iliyan
 */
 
 #ifndef RTFACT__ALLOCATIONUTILS_HPP
#define RTFACT__ALLOCATIONUTILS_HPP

// Note: this file is included by RTfact/Config/Common.hpp

#include <memory>

RTFACT_NAMESPACE_BEGIN

#define RTFACT_ALLOCATOR_INLINE RTFACT_FORCE_INLINE

template<
    typename tElement>
class AlignedAllocator :
   public std::allocator<tElement>
{
    typedef std::allocator<tElement> t_Base;

    enum
    {
        REAL_DATA_ALIGNMENT = RTFACT_ALIGN_OF(tElement)
    };

    //RTFACT_STATIC_ASSERT(sizeof(tElement) % REAL_DATA_ALIGNMENT == 0);

public:

    typedef size_t          size_type;
    typedef ptrdiff_t       difference_type;
    typedef tElement*       pointer;
    typedef const tElement* const_pointer;
    typedef tElement&       reference;
    typedef const tElement& const_reference;
    typedef tElement        value_type;

    template<
        class tOther>
    struct rebind 
    {
        typedef AlignedAllocator<tOther> other;
    };

    RTFACT_ALLOCATOR_INLINE AlignedAllocator()
    {}

    template<
        class tOther>
    RTFACT_ALLOCATOR_INLINE AlignedAllocator(
        const AlignedAllocator<tOther>& aAllocator)
    {}

    RTFACT_ALLOCATOR_INLINE AlignedAllocator& operator=(
        const AlignedAllocator& aAllocator)
    {
        t_Base::operator=(aAllocator);

        return *this;
    }

    RTFACT_ALLOCATOR_INLINE pointer allocate(
        size_type aSize,
        const void* = 0)
    {
        const size_type byteCount = sizeof(tElement) * aSize;

        pointer ret = reinterpret_cast<pointer>(
            alignedMalloc(byteCount, REAL_DATA_ALIGNMENT));

        if(ret == RTFACT_NULL)
        {
            throw std::bad_alloc();
        }

        return ret;
    }

    RTFACT_ALLOCATOR_INLINE void deallocate(
        pointer aPtr,
        size_type)
    {
          alignedFree(aPtr);
    }

    RTFACT_ALLOCATOR_INLINE void construct(
        pointer aPtr,
        const tElement &aVal)
    {
        new(aPtr) tElement(aVal);
    }

    void destroy(
        pointer aPtr)
    {
        aPtr->~tElement();
    }
};

#define RTFACT_ALIGNED_NEW_OPERATORS(aAlignment)                               \
    RTFACT_ALLOCATOR_INLINE void* operator new(                                \
        size_t aSize)                                                          \
    {                                                                          \
        return ::RTfact::alignedMalloc(aSize, aAlignment);                     \
    }                                                                          \
                                                                               \
    RTFACT_ALLOCATOR_INLINE void* operator new[](                              \
        size_t aSize)                                                          \
    {                                                                          \
        return ::RTfact::alignedMalloc(aSize, aAlignment);                     \
    }                                                                          \
                                                                               \
    RTFACT_ALLOCATOR_INLINE void* operator new(                                \
        size_t,                                                                \
        void* aPtr)                                                            \
    {                                                                          \
        RTFACT_ASSERT(reinterpret_cast<size_t>(aPtr) % aAlignment == 0);       \
                                                                               \
        return aPtr;                                                           \
    }                                                                          \
                                                                               \
    RTFACT_ALLOCATOR_INLINE void* operator new[](                              \
        size_t,                                                                \
        void* aPtr)                                                            \
    {                                                                          \
        RTFACT_ASSERT(reinterpret_cast<size_t>(aPtr) % aAlignment == 0);       \
                                                                               \
        return aPtr;                                                           \
    }                                                                          \
                                                                               \
    RTFACT_ALLOCATOR_INLINE void operator delete(                              \
        void* aPtr)                                                            \
    {                                                                          \
        ::RTfact::alignedFree(aPtr);                                           \
    }                                                                          \
                                                                               \
    RTFACT_ALLOCATOR_INLINE void operator delete(                              \
        void*,                                                                 \
        void*)                                                                 \
    {}                                                                         \
                                                                               \
    RTFACT_ALLOCATOR_INLINE void operator delete(                              \
        void* aPtr,                                                            \
        size_t)                                                                \
    {                                                                          \
        ::RTfact::alignedFree(aPtr);                                           \
    }                                                                          \
                                                                               \
    RTFACT_ALLOCATOR_INLINE void operator delete[](                            \
        void* aPtr)                                                            \
    {                                                                          \
        ::RTfact::alignedFree(aPtr);                                           \
    }                                                                          \
                                                                               \
    RTFACT_ALLOCATOR_INLINE void operator delete[](                            \
        void*,                                                                 \
        void*)                                                                 \
    {}                                                                         \
                                                                               \
    RTFACT_ALLOCATOR_INLINE void operator delete[](                            \
        void* aPtr,                                                            \
        size_t)                                                                \
    {                                                                          \
        ::RTfact::alignedFree(aPtr);                                           \
    }    

RTFACT_NAMESPACE_END

#endif // RTFACT__ALLOCATIONUTILS_HPP
