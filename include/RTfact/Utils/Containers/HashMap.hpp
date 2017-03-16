/*
 *  HashMap.hpp
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

 #ifndef RTFACT__HASHMAPUTILS_HPP
#define RTFACT__HASHMAPUTILS_HPP

#include <RTfact/Config/Common.hpp>

#if defined(RTFACT_COMPILER_GCC) && RTFACT_COMPILER_GCC_VERSION >= 40300 && \
    defined(__INTEL_COMPILER) && __INTEL_COMPILER >= 1200
#   include <tr1/unordered_map>
#elif defined(RTFACT_COMPILER_GCC) && RTFACT_COMPILER_GCC_VERSION > 30200
#   include <ext/hash_map>
#elif defined(RTFACT_COMPILER_MSVC)
#   include <hash_map>
#else
#   error Please specify the details of hash_map specific to your compiler
#endif

#if defined(RTFACT_COMPILER_GCC) && RTFACT_COMPILER_GCC_VERSION >= 40300 && \
    defined(__INTEL_COMPILER) && __INTEL_COMPILER >= 1200

#   define RTFACT__HASH_MAP_CLASS unordered_map
#   define RTFACT__HASH_MAP_BASE_NAMESPACE std::tr1

#elif defined(RTFACT_COMPILER_GCC) && RTFACT_COMPILER_GCC_VERSION > 30200

#   define RTFACT__HASH_MAP_BASE_NAMESPACE __gnu_cxx
#   define RTFACT__HASH_MAP_CLASS hash_map

    namespace RTFACT__HASH_MAP_BASE_NAMESPACE
    {
        template<>
        struct hash<void *>
        {
            union conv
            {
                size_t s;
                const void  *p;
            };

            RTFACT_INLINE size_t operator()(
                void* x) const
            {
                conv c;
                c.p = x;
                return c.s;
            }
        };

        template<>
        struct hash<const void *>
        {
            union conv
            {
                size_t s;
                const void  *p;
            };

            RTFACT_INLINE size_t operator()(
                const void* x) const
            {
                conv c;
                c.p = x;
                return c.s;
            }
        };

        template<>
        struct hash<const void * const>
        {
            union conv
            {
                size_t s;
                const void  *p;
            };

            RTFACT_INLINE size_t operator()(
                const void* const x) const
            {
                conv c;
                c.p = x;
                return c.s;
            }
        };

        template<>
        struct hash<std::string>
        {
            RTFACT_INLINE size_t operator()(
                const std::string &s) const
            {
                hash<const char *> h;
                return h(s.c_str());
            }
        };
    }

#elif defined(RTFACT_COMPILER_MSVC)

#   define RTFACT__HASH_MAP_BASE_NAMESPACE stdext
#   define RTFACT__HASH_MAP_CLASS hash_map

#endif

RTFACT_NAMESPACE_BEGIN

template<
    class tKey,
    class tValue>
class HashMap : public RTFACT__HASH_MAP_BASE_NAMESPACE::RTFACT__HASH_MAP_CLASS<tKey, tValue>
{
    typedef RTFACT__HASH_MAP_BASE_NAMESPACE::RTFACT__HASH_MAP_CLASS<tKey, tValue> t_Base;


public:

    typedef typename t_Base::allocator_type         allocator_type;
    typedef typename t_Base::const_iterator         const_iterator;
    typedef typename t_Base::const_pointer          const_pointer;
    typedef typename t_Base::const_reference        const_reference;
    //typedef typename t_Base::const_reverse_iterator const_reverse_iterator;
    typedef typename t_Base::difference_type        difference_type;
    typedef typename t_Base::iterator               iterator;
    //typedef typename t_Base::key_compare            key_compare;
    typedef typename t_Base::key_type               key_type;
    typedef typename t_Base::mapped_type            mapped_type;
    typedef typename t_Base::pointer                pointer;
    typedef typename t_Base::reference              reference;
    //typedef typename t_Base::reverse_iterator       reverse_iterator;
    typedef typename t_Base::size_type              size_type;
    typedef typename t_Base::value_type             value_type;

    HashMap() :
        t_Base()
    {}

    HashMap(
        const HashMap& aOther
    ) :
        t_Base(aOther)
    {}

    HashMap<tKey, tValue>& operator=(
        const HashMap<tKey, tValue>& aOther)
    {
        t_Base::operator=(aOther);

        return *this;
    }
};

RTFACT_NAMESPACE_END

#endif // RTFACT__HASHMAPUTILS_HPP
