/* 
 *  FastVector.hpp
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
 *  Author(s): Iliyan Georgiev, Felix Klein
 */
 
 #ifndef RTFACT__FASTVECTOR_HPP
#define RTFACT__FASTVECTOR_HPP

#include <RTfact/Config/Common.hpp>

#include <boost/type_traits/has_trivial_constructor.hpp>
#include <boost/type_traits/has_trivial_destructor.hpp>
#include <boost/type_traits/has_trivial_assign.hpp>
#include <boost/type_traits/integral_constant.hpp>

#include <RTfact/Utils/Containers/Detail/ArrayUtils.hpp>

#define RTFACT_FAST_VECTOR_INLINE RTFACT_CONTAINER_INLINE
#define RTFACT_FAST_VECTOR_ASSERT RTFACT_ASSERT

RTFACT_NAMESPACE_BEGIN

template<
    class tElement>
class FastVector
{
public:

    typedef RTfact::AlignedAllocator<tElement> allocator_type;
    typedef tElement                           value_type;
    typedef const tElement&                    const_reference;
    typedef size_t                             size_type;
    typedef tElement*                          pointer;
    typedef const tElement*                    const_pointer;
    typedef tElement&                          reference;
    typedef pointer                            iterator;
    typedef const_pointer                      const_iterator;
    typedef ptrdiff_t                          difference_type;

private:

    enum
    {
        HAS_TRIVIAL_CONSTRUCTOR = boost::has_trivial_constructor<value_type>::value,
        HAS_TRIVIAL_DESTRUCTOR  = boost::has_trivial_destructor<value_type>::value,
        HAS_TRIVIAL_ASSIGNMENT  = boost::has_trivial_assign<value_type>::value
    };

    pointer mBegin, mEnd, mCapacityEnd;

    allocator_type mAllocator;

    RTFACT_INLINE void construct(
        pointer aElement)
    {
        if(!HAS_TRIVIAL_CONSTRUCTOR)
        {
            new(aElement) value_type;
        }
    }

    RTFACT_INLINE void construct(
        pointer aElement,
        const_reference aValue)
    {
        if(HAS_TRIVIAL_ASSIGNMENT)
        {
            *aElement = aValue;
        }
        else
        {
            new(aElement) value_type(aValue);
        }
    }

    RTFACT_INLINE void constructRange(
        pointer aBegin,
        pointer aEnd)
    {
        RTFACT_FAST_VECTOR_ASSERT(aBegin <= aEnd);

        if(!HAS_TRIVIAL_CONSTRUCTOR)
        {
            for(pointer it = aBegin; it != aEnd; ++it)
            {
                new (it) tElement;
            }
        }
    }
    
    RTFACT_INLINE void destroyRange(
        pointer aBegin,
        pointer aEnd)
    {
        RTFACT_FAST_VECTOR_ASSERT(aBegin <= aEnd);

        if(!HAS_TRIVIAL_DESTRUCTOR)
        {
            for(pointer it = aBegin; it != aEnd; ++it)
            {
                it->tElement::~tElement();
            }
        }
    }

    template<
        typename tSource,
        typename tResult>
    RTFACT_INLINE tResult copy(
        tSource aBegin,
        tSource aEnd,
        tResult aResult)
    {
        RTFACT_FAST_VECTOR_ASSERT(aBegin <= aEnd);

        return Detail::fastCopy(aBegin, aEnd, aResult);
    }

    RTFACT_INLINE void reserveAtLeast(
        const size_type aCount)
    {
        reserve(std::max(aCount, (size_type)capacity() * 3 / 2));
    }

public:

    FastVector() : 
        mBegin(RTFACT_NULL),
        mEnd(RTFACT_NULL),
        mCapacityEnd(RTFACT_NULL)
    {}

    FastVector(
        const FastVector<tElement>& aOther
    ) :
        mBegin(RTFACT_NULL),
        mEnd(RTFACT_NULL),
        mCapacityEnd(RTFACT_NULL),
        mAllocator(aOther.mAllocator)
    {
        reserveAtLeast(aOther.size());

        copy(aOther.mBegin, aOther.mEnd, mBegin);
        mEnd += aOther.size();
    }

    FastVector(
        const size_type aSize
    ) :
        mBegin(RTFACT_NULL),
        mEnd(RTFACT_NULL),
        mCapacityEnd(RTFACT_NULL)
    {
        resize(aSize);
    }

    ~FastVector()
    {
        clear();
    }

    FastVector<tElement>& operator=(
        const FastVector<tElement>& aOther)
    {
        clear();
        reserveAtLeast(aOther.size());

        copy(aOther.mBegin, aOther.mEnd, mBegin);
        mEnd += aOther.size();

        return *this;
    }

    allocator_type get_allocator() const
    {
        return mAllocator;
    }

    RTFACT_FAST_VECTOR_INLINE size_type capacity() const
    {
        return mCapacityEnd - mBegin;
    }

    void reserve(
        size_type aCount)
    {
        if(aCount > capacity())
        {
            pointer oldFBegin = mBegin, oldEnd = mEnd, oldCapacityEnd = mCapacityEnd;
            size_type oldSize = mEnd - mBegin;

            mBegin = mAllocator.allocate(aCount);

            mEnd = mBegin + oldSize;
            mCapacityEnd = mBegin + aCount;

            constructRange(mBegin, mCapacityEnd);

            copy(oldFBegin, oldEnd, mBegin);
            destroyRange(oldFBegin, oldEnd);
            mAllocator.deallocate(oldFBegin, oldCapacityEnd - oldFBegin);
        }
    }

    RTFACT_FAST_VECTOR_INLINE void resize(
        size_type aSize)
    {
        if(aSize >= capacity())
        {
            //try to grow by 50%, aCount at least
            size_type newSize =
                std::max((size_type)(mCapacityEnd - mBegin) * 3 / 2, aSize);

            reserve(newSize);
        }

        pointer oldEnd = mEnd;
        mEnd = mBegin + aSize;

        if(oldEnd < mEnd)
        {
            constructRange(oldEnd, mEnd);
        }
        else if(oldEnd > mEnd)
        {
            destroyRange(mEnd, oldEnd);
        }
    }

    void clear()
    {
        if(size() > 0)
        {
            destroyRange(mBegin, mEnd);
        }

        if(capacity() > 0)
        {
            mAllocator.deallocate(mBegin, mCapacityEnd - mBegin);
        }

        mBegin = RTFACT_NULL;
        mEnd = RTFACT_NULL;
        mCapacityEnd = RTFACT_NULL;
    }

    RTFACT_FAST_VECTOR_INLINE size_type size() const
    {
        return mEnd - mBegin;
    }

    RTFACT_FAST_VECTOR_INLINE bool empty() const
    {
        return mBegin == mEnd;
    }

    RTFACT_FAST_VECTOR_INLINE void push_back(
        const_reference aElement)
    {
        if(size() >= capacity())
        {
            reserveAtLeast(size() + 1);
        }

        construct(mEnd, aElement);

        ++mEnd;
    }

    template<
        class tIterator>
    void insert(
        iterator aWhere,
        tIterator aBegin,
        tIterator aEnd)
    {
        RTFACT_FAST_VECTOR_ASSERT(aBegin <= aEnd);
        RTFACT_FAST_VECTOR_ASSERT(aWhere == mEnd);
        RTFACT_FAST_VECTOR_ASSERT(aWhere >= mBegin && aWhere <= mEnd);

        size_type seqLen = aEnd - aBegin;
        size_type whereOffs = aWhere - mBegin;
        size_type lastOffs = mEnd - mBegin;

        if(seqLen + size() >= capacity())
        {
            reserveAtLeast(size() + seqLen);
        }

        if(whereOffs != lastOffs)
        {
            copy(mBegin + whereOffs + seqLen,
                 mBegin + lastOffs,
                 mBegin + whereOffs + seqLen);
        }

        copy(aBegin, aEnd, mBegin + whereOffs);
        mEnd += seqLen;
    }

    iterator erase(
        const_iterator aWhere)
    {
        RTFACT_FAST_VECTOR_ASSERT(!empty());

        iterator where = const_cast<iterator>(aWhere);

        copy(where + 1, mEnd, where);
        destroyRange(mEnd - 1, mEnd);

        --mEnd;

        return const_cast<iterator>(aWhere);
    }

    iterator erase(
        const_iterator aWhereBegin,
        const_iterator aWhereEnd)
    {
        RTFACT_FAST_VECTOR_ASSERT(!empty());
        RTFACT_FAST_VECTOR_ASSERT(aWhereBegin < aWhereEnd);

        if(aWhereBegin != aWhereEnd)
        {
            iterator begin = const_cast<iterator>(aWhereBegin);
            iterator end = const_cast<iterator>(aWhereEnd);

            copy(end, mEnd, begin);

            const ptrdiff_t diff = end - begin;

            destroyRange(mEnd - diff, mEnd);

            mEnd -= diff;
        }

        return const_cast<iterator>(aWhereBegin);
    }

    void pop_back()
    {
        if(!empty())
        {
            erase(mEnd - 1);
        }
    }

    RTFACT_FAST_VECTOR_INLINE iterator begin()
    {
        return mBegin;
    }

    RTFACT_FAST_VECTOR_INLINE const_iterator begin() const
    {
        return mBegin;
    }

    RTFACT_FAST_VECTOR_INLINE iterator end()
    {
        return mEnd;
    }

    RTFACT_FAST_VECTOR_INLINE const_iterator end() const
    {
        return mEnd;
    }

    RTFACT_FAST_VECTOR_INLINE reference front()
    {
        RTFACT_FAST_VECTOR_ASSERT(mBegin != RTFACT_NULL);

        return *mBegin;
    }

    RTFACT_FAST_VECTOR_INLINE const_reference front() const
    {
        RTFACT_FAST_VECTOR_ASSERT(mBegin != RTFACT_NULL);

        return *mBegin;
    }

    RTFACT_FAST_VECTOR_INLINE reference back()
    {
        RTFACT_FAST_VECTOR_ASSERT(mBegin != RTFACT_NULL);

        return *(mEnd - 1);
    }

    RTFACT_FAST_VECTOR_INLINE const_reference back() const
    {
        RTFACT_FAST_VECTOR_ASSERT(mBegin != RTFACT_NULL);

        return *(mEnd - 1);
    }

    RTFACT_FAST_VECTOR_INLINE reference operator[](
        size_type aIndex)
    {
        RTFACT_FAST_VECTOR_ASSERT(aIndex < size());

        return mBegin[aIndex];
    }
 
    RTFACT_FAST_VECTOR_INLINE const_reference operator[](
        size_type aIndex) const
    {
        RTFACT_FAST_VECTOR_ASSERT(aIndex < size());

        return mBegin[aIndex];
    }
};

RTFACT_NAMESPACE_END

#undef RTFACT_FAST_VECTOR_INLINE
#undef RTFACT_FAST_VECTOR_ASSERT

#endif // RTFACT__FASTVECTOR_HPP
