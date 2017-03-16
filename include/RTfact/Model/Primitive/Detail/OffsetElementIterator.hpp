/* 
 *  OffsetElementIterator.hpp
 *
 *  RTfact - Real-Time Ray Tracing Library
 *
 *  Copyright (C) 2008  Saarland University
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
 *  Created on: 2008-06-09 17:30:26 +0200
 *  Author(s): Iliyan Georgiev
 */
 
 #ifndef RTFACT__OFFSETELEMENTITERATOR_HPP
#define RTFACT__OFFSETELEMENTITERATOR_HPP

#include <RTfact/Config/Common.hpp>

RTFACT_NAMESPACE_BEGIN

namespace Detail {

template<
    class tElement,
    class tOffsetIterator>
class OffsetElementIterator
{
public:

    typedef tElement Element;

private:

    Element* mElementBase;
    tOffsetIterator mOffsetIterator;

public:

    OffsetElementIterator()
    {}

    OffsetElementIterator(
        tElement* aElementBase,
        tOffsetIterator aOffsetIterator
    ) :
        mElementBase(aElementBase),
        mOffsetIterator(aOffsetIterator)
    {}

    RTFACT_PRIMITIVE_INLINE Element& operator*() const
    {
        return *(mElementBase + *mOffsetIterator);
    }

    RTFACT_PRIMITIVE_INLINE OffsetElementIterator& operator++()
    {
        ++mOffsetIterator;

        return *this;
    }

    RTFACT_PRIMITIVE_INLINE OffsetElementIterator operator++(int)
    {
        OffsetElementIterator prevThis = *this;

        ++mOffsetIterator;

        return prevThis;
    }

    RTFACT_PRIMITIVE_INLINE bool operator==(
        const OffsetElementIterator& aOther) const
    {
        RTFACT_PRIMITIVE_ASSERT(mElementBase == aOther.mElementBase);

        return (mOffsetIterator == aOther.mOffsetIterator);
    }

    RTFACT_PRIMITIVE_INLINE bool operator!=(
        const OffsetElementIterator& aOther) const
    {
        RTFACT_PRIMITIVE_ASSERT(mElementBase == aOther.mElementBase);

        return (mOffsetIterator != aOther.mOffsetIterator);
    }

    RTFACT_PRIMITIVE_INLINE size_t operator-(
        const OffsetElementIterator& aOther) const
    {
        RTFACT_PRIMITIVE_ASSERT(mElementBase == aOther.mElementBase);

        return (mOffsetIterator - aOther.mOffsetIterator);
    }
};

} // namespace Detail

RTFACT_NAMESPACE_END

#endif // RTFACT__OFFSETELEMENTITERATOR_HPP
