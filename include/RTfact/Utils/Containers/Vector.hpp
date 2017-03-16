/* 
 *  Vector.hpp
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
 *  Created on: 2008-11-02 21:38:37 +0100
 *  Author(s): Iliyan Georgiev, Felix Klein
 */
 
 #ifndef RTFACT__VECTOR_HPP
#define RTFACT__VECTOR_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Utils/IO/SerializationUtils.hpp>

#if (defined(RTFACT_DEBUG) && !defined(RTFACT_VECTOR_USE_FAST_VECTOR))         \
        || defined(RTFACT_VECTOR_USE_STD_VECTOR)
#   include <vector>
#   define RTFACT__VECTOR_BASE(aType)                                          \
        std::vector<aType, AlignedAllocator<aType> >
#else
#   include <RTfact/Utils/Containers/FastVector.hpp>
#   define RTFACT__VECTOR_BASE(aType)                                          \
        FastVector<aType >
#endif

RTFACT_NAMESPACE_BEGIN

template<
    class tElement>
class Vector :
    public RTFACT__VECTOR_BASE(tElement)
{
    typedef RTFACT__VECTOR_BASE(tElement) t_Base;

public:

    typedef typename t_Base::allocator_type  allocator_type;
    typedef typename t_Base::value_type      value_type;
    typedef typename t_Base::const_reference const_reference;
    typedef typename t_Base::size_type       size_type;
    typedef typename t_Base::pointer         pointer;
    typedef typename t_Base::const_pointer   const_pointer;
    typedef typename t_Base::reference       reference;
    typedef typename t_Base::iterator        iterator;
    typedef typename t_Base::const_iterator  const_iterator;
    typedef typename t_Base::difference_type difference_type;

    //using t_Base::size;

    RTFACT_FORCE_INLINE Vector() :
        t_Base()
    {}

    RTFACT_FORCE_INLINE Vector(
        size_type aSize
    ) :
        t_Base(aSize)
    {}

    RTFACT_FORCE_INLINE Vector(
        const Vector<tElement>& aOther
    ) :
        t_Base(aOther)
    {}

    RTFACT_FORCE_INLINE Vector<tElement>& operator=(
        const Vector<tElement>& aOther)
    {
        t_Base::operator=(aOther);

        return *this;
    }

    void save(
        IO::Serializer& aSerializer)
    {
        size_type s = t_Base::size();
        aSerializer << s;

        for(iterator it = t_Base::begin(); it != t_Base::end(); ++it)
        {
            aSerializer << *it;
        }
    }

    void load(
        IO::Serializer& aSerializer)
    {
        size_type size;
        aSerializer >> size;

        resize(size);

        for(iterator it = t_Base::begin(); it != t_Base::end(); ++it)
        {
            aSerializer >> *it;
        }
    }
};

RTFACT_NAMESPACE_END

RTFACT_SERIALIZATION_REGISTER_LOAD_SAVE_METHODS_TEMPLATE_CLASS(RTfact::Vector, (class))

#undef RTFACT__VECTOR_BASE

#endif // RTFACT__VECTOR_HPP
