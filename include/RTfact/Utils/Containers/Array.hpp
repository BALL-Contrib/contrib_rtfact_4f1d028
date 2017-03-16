/* 
 *  Array.hpp
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
 
 #ifndef RTFACT__ARRAY_HPP
#define RTFACT__ARRAY_HPP

#include <RTfact/Config/Common.hpp>

RTFACT_NAMESPACE_BEGIN

template<
    uint taSize,
    class tElement>
class Array
{
public:

    typedef tElement       value_type;
    typedef value_type*    iterator;
    typedef const iterator const_iterator;

    enum
    {
        SIZE = taSize
    };

    value_type data[SIZE];

    Array()
    {}

    RTFACT_FORCE_INLINE explicit Array(
        const value_type& aValue)
    {
        //TODO: can be optimized for primitive types
        for(uint i = 0; i < SIZE; ++i)
        {
            data[i] = aValue;
        }
    }

    RTFACT_FORCE_INLINE const value_type& operator[](
        const uint aIndex) const
    {
        RTFACT_ASSERT(aIndex < SIZE);

        return data[aIndex];
    }

    RTFACT_FORCE_INLINE value_type& operator[](
        const uint aIndex)
    {
        RTFACT_ASSERT(aIndex < SIZE);

        return data[aIndex];
    }

    RTFACT_FORCE_INLINE const_iterator begin() const
    {
        return data;
    }

    RTFACT_FORCE_INLINE iterator begin()
    {
        return data;
    }

    RTFACT_FORCE_INLINE const_iterator end() const
    {
        return data + SIZE;
    }

    RTFACT_FORCE_INLINE iterator end()
    {
        return data + SIZE;
    }
};

RTFACT_NAMESPACE_END

#endif // RTFACT__ARRAY_HPP
