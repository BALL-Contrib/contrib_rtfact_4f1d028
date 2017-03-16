/* 
 *  ArrayUtils.hpp
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
 *  Author(s): Iliyan Georgiev
 */
 
 #ifndef RTFACT__ARRAYUTILS_HPP
#define RTFACT__ARRAYUTILS_HPP

#include <RTfact/Config/Common.hpp>

#include <boost/type_traits/has_trivial_assign.hpp>

RTFACT_NAMESPACE_BEGIN

// fast_copy from boost example of optimized version of std::copy
namespace Detail {

template<
    class tElement,
    bool taOwnData>
struct MemoryDeallocator
{
    void operator()(
        tElement* aPtr) const
    {
        if(taOwnData)
        {
            delete [] aPtr;
        }
    }
};

template<
    class tSource,
    class tResult,
    bool b>
RTFACT_INLINE tResult fast_copy_imp(
    tSource first,
    tSource last,
    tResult out, 
    const boost::integral_constant<bool, b>&)
{
    while(first != last)
    {
        *out = *first;
        ++out;
        ++first;
    }
    return out;
}

template<
    class T>
RTFACT_INLINE T* fast_copy_imp(
    const T* first,
    const T* last,
    T* out, 
    const boost::true_type&)
{
    memcpy(out, first, (last - first) * sizeof(T));
    return out + (last - first);
}

template<
    class tSource,
    class tResult>
RTFACT_INLINE tResult fastCopy(
    tSource aBegin,
    tSource aEnd,
    tResult oResult)
{
    //
    // We can copy with memcpy if T has a trivial assignment operator,
    // and if the iterator arguments are actually pointers (this last
    // requirement we detect with overload resolution):
    //
    typedef typename std::iterator_traits<tSource>::value_type value_type;

    return Detail::fast_copy_imp(
        aBegin, aEnd, oResult, boost::has_trivial_assign<value_type>());
}

} // namespace Detail

RTFACT_NAMESPACE_END

#endif // RTFACT__ARRAYUTILS_HPP
