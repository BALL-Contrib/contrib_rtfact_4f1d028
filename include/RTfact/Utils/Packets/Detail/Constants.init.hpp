/*
 *  Constants.init.hpp
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
 *  Created on: 2009-03-06 23:28:11 +0100
 *  Author(s): Iliyan Georgiev, Felix Klein
 */

 #ifndef RTFACT__CONSTANTS_INC_CPP
#define RTFACT__CONSTANTS_INC_CPP

#include <RTfact/Config/Common.hpp>

#include <boost/preprocessor/iteration/local.hpp>

#include <RTfact/Utils/Packets/Packet.hpp>
#include <RTfact/Utils/Packets/BitMask.hpp>

RTFACT_NAMESPACE_BEGIN

RTFACT_SSE_INIT_CONSTS

#define RTFACT__INIT_CONSTS(aIndex)                                            \
    RTFACT_PACKET_INIT_CONSTS(RTFACT_PACKET_SIZE_AT(aIndex))                   \
    RTFACT_MASK_INIT_CONSTS(RTFACT_PACKET_SIZE_AT(aIndex))

#define  BOOST_PP_LOCAL_MACRO(n)  RTFACT__INIT_CONSTS(n)
#define  BOOST_PP_LOCAL_LIMITS    (0, BOOST_PP_SEQ_SIZE(RTFACT_PACKET_SIZES)-1)
#include BOOST_PP_LOCAL_ITERATE()

#undef  RTFACT__INIT_CONSTS
#undef  BOOST_PP_LOCAL_MACRO
#undef  BOOST_PP_LOCAL_LIMITS

RTFACT_NAMESPACE_END

/* NOTE: Packet constants generating must precede Vec3f.hpp inclusion
   since it uses packets' constants inside its methods. Clang does not
   like it. MSVC, on the other hand, does not like forward-like
   declarations of constant static members, which might also solve
   the situation.
*/

#include <RTfact/Utils/Packets/Vec3f.hpp>

RTFACT_NAMESPACE_BEGIN

#define RTFACT__INIT_CONSTS(aIndex)                                            \
    RTFACT_VEC3F_INIT_CONSTS(RTFACT_PACKET_SIZE_AT(aIndex))                    \

#define  BOOST_PP_LOCAL_MACRO(n)  RTFACT__INIT_CONSTS(n)
#define  BOOST_PP_LOCAL_LIMITS    (0, BOOST_PP_SEQ_SIZE(RTFACT_PACKET_SIZES)-1)
#include BOOST_PP_LOCAL_ITERATE()

namespace Detail {

    extern const RTFACT_ALIGN(16) int32 _Sign_PNNP[4];

    // Used in Matrix4f.hpp
    const RTFACT_ALIGN(16) int32 _Sign_PNNP[4] =
	{ static_cast<int32>(0x00000000), static_cast<int32>(0x80000000), static_cast<int32>(0x80000000), static_cast<int32>(0x00000000) };

} // namespace Detail

RTFACT_NAMESPACE_END

#endif // RTFACT__CONSTANTS_INC_CPP
