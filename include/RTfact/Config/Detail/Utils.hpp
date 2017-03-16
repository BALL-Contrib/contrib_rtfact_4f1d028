/* 
 *  Utils.hpp
 *
 *  RTfact - Real-Time Ray Tracing Library
 *
 *  Copyright (C) 2010  Saarland University
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
 *  Created on: 2009-01-29 19:31:16 +0100
 *  Author(s): Iliyan Georgiev, Lukas Marsalek, Petr Kmoch
 */
 
 #ifndef RTFACT__CONFIG_UTILS_HPP
#define RTFACT__CONFIG_UTILS_HPP

#define RTFACT_DEFINE_GET_PROPERTY(aType, aName)                               \
    const aType get##aName() const                                             \
    {                                                                          \
        return m##aName;                                                       \
    }

#define RTFACT_DEFINE_SET_PROPERTY(aType, aName)                               \
    void set##aName(                                                           \
        const aType a##aName)                                                  \
    {                                                                          \
        m##aName = a##aName;                                                   \
    }

#define RTFACT_DEFINE_PROPERTY(aType, aName)                                   \
    RTFACT_DEFINE_GET_PROPERTY(aType, aName)                                   \
    RTFACT_DEFINE_SET_PROPERTY(aType, aName)

#pragma region Utility macros

#define RTFACT_UNREFERENCED_PARAMETER(aParameter) (aParameter)

#define RTFACT_CHECK_UMASK(aMask, aValue) (((aMask) & (aValue)) == (aValue))

#define RTFACT_MIN(a, b) ((a) < (b) ? (a) : (b))
#define RTFACT_MAX(a, b) ((a) > (b) ? (a) : (b))

#define RTFACT_MAX_DIMENSION(aX, aY, aZ)                                       \
    ((aX > aY) ? ((aX > aZ) ? 0 : 2) : ((aY > aZ) ? 1 : 2))

#define RTFACT_MIN_DIMENSION(aX, aY, aZ)                                       \
    ((aX < aY) ? ((aX < aZ) ? 0 : 2) : ((aY < aZ) ? 1 : 2))

#define RTFACT_STATIC_CONSTANT(aName, aValue) enum { aName = (aValue) }

#if defined(RTFACT_COMPILER_MSVC) || defined(RTFACT_COMPILER_INTEL)
#   define RTFACT_ALIGN(aBorder) __declspec(align(aBorder))
#elif defined(RTFACT_COMPILER_GCC)
#   define RTFACT_ALIGN(aBorder) __attribute__ ((aligned (aBorder)))
#else
#   error Please define a type alignment macro
#endif

#define RTFACT_ALIGN_CACHE_LINE RTFACT_ALIGN(RTFACT_CACHE_LINE_SIZE)
#define RTFACT_ALIGN_SIMD       RTFACT_ALIGN(RTFACT_SIMD_WIDTH_BYTES)

#define RTFACT_SIMD_ALIGNED_NEW_OPERATORS                                      \
    RTFACT_ALIGNED_NEW_OPERATORS(RTFACT_SIMD_WIDTH_BYTES)

#define RTFACT_ALIGN_OF(aType)                                                 \
    (__alignof(aType) > 0 ? RTFACT_MAX(__alignof(aType), sizeof(void *)) : 1)

#define RTFACT_DOWNSIZE_PACKET_CAST(aType,aExpression)                         \
    aType(aExpression)

#pragma endregion // Utility macros

#pragma region Packet settings

#ifndef RTFACT_PACKET_SIZES
#   define RTFACT_PACKET_SIZES (1)(4)(16)(64)(256)
#endif

/*  Notes about RTFACT_PACKET_SIZES:
        
    1)  The list is used for automatic constant
        initialization and source code generation;
    2)  For optimal performance and smaller code, put in
        the list only the sizes that are actually used
        throughout the application;
    3)  Preserve the list's syntax;
    4)  The emelent order doesn't matter;
    5)  Do not remove 1 from the list.
*/

#define RTFACT_PACKET_SIZE_AT(aIndex)                                          \
    BOOST_PP_SEQ_ELEM(aIndex, RTFACT_PACKET_SIZES)

#define RTFACT_PACKET_SIZE_AT(aIndex)                                          \
    BOOST_PP_SEQ_ELEM(aIndex, RTFACT_PACKET_SIZES)

#define RTFACT_PACKET_SIZES_SIZE                                               \
    BOOST_PP_SEQ_SIZE(RTFACT_PACKET_SIZES)

#pragma endregion // Packet settings

#pragma region Additional component settings

//#define RTFACT_NO_USER_WARNINGS
//#define RTFACT_NO_PROGRAM_WARNINGS

// Disable output of warnings/errors/messages to standard output streams?
//#define RTFACT_NO_STD_OUTPUT

// Faster (but less accurate) SSE operations?
//#define RTFACT_SSE_FAST_RECIPROCAL
//#define RTFACT_SSE_FAST_RECIP_SQRT

// Does inverse() of a singular matrix zero the matrix?
//#define RTFACT_MATRIX_SINGULAR_INVERSE_ZERO

#if defined(RTFACT_VECTOR_USE_FAST_VECTOR) && defined(RTFACT_VECTOR_USE_STD_VECTOR)
#   error RTFACT_VECTOR_USE_FAST_VECTOR and RTFACT_VECTOR_USE_STD_VECTOR are incompatible
#endif

//#define RTFACT_USE_MAILBOXING

#ifndef RTFACT_TRAVERSER_MAX_STACK_DEPTH
#   define RTFACT_TRAVERSER_MAX_STACK_DEPTH 128
#endif

#pragma endregion // Additional component settings

RTFACT_NAMESPACE_BEGIN

template<
    class tType>
struct Traits
{
    typedef boost::intrusive_ptr<tType> Ptr;
};

template <
    typename tType>
struct Traits<boost::intrusive_ptr<tType> >
{
    typedef boost::intrusive_ptr<boost::intrusive_ptr<tType> > Ptr;
    typedef tType Base;
};

struct TrueType
{
    enum { VALUE = 1 };
};

struct FalseType
{
    enum { VALUE = 0 };
};

template<
bool taCondition>
struct ConditionType :
    public TrueType
{
    using TrueType::VALUE;
};

template<>
struct ConditionType<false> :
    public FalseType
{
    using FalseType::VALUE;
};

RTFACT_NAMESPACE_END

#endif // RTFACT__CONFIG_UTILS_HPP
