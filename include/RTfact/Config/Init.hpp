/* 
 *  Init.hpp
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
 *  Author(s): Iliyan Georgiev
 */
 
 #ifndef RTFACT__INIT_INC_CPP
#define RTFACT__INIT_INC_CPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Utils/StaticInitializer.hpp>

#include <RTfact/Utils/Math/Detail/PrimeNumberUtils.init.hpp>
#include <RTfact/Utils/Math/Detail/FaurePermutationUtils.init.hpp>
#include <RTfact/Utils/Packets/Detail/Constants.init.hpp>

RTFACT_NAMESPACE_BEGIN

namespace Detail {
namespace
{
    bool RTFACT__FAST_SSE_DENORMALS_ENABLED = enableFastSSEDenormals();

    bool RTFACT__STATIC_INITIALIZATION_PERFORMED =
        StaticInitializer::performInitialization();
}
} // namespace Detail

RTFACT_NAMESPACE_END

#endif // RTFACT__INIT_INC_CPP
