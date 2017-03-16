/*
 *  IntersectTaskHandle.hpp
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
 *  Created on: Sun Oct 14 16:27:00 CEST 2012 @643
 *  Author(s): Vaclav Krajicek
 */

 #ifndef RTFACT_HANDLE_INTERSECTTASK_HPP
#define RTFACT_HANDLE_INTERSECTTASK_HPP

#include <RTpieCpp/Config/Common.hpp>

#include <RTpie/Tasks/IIntersectTask.hpp>

#define IMPL_INTERFACE RTfact::RTpie::IIntersectTask
WRAPPER_FWD(IntersectTaskHandle)
#undef IMPL_INTERFACE

#include <RTpieCpp/Base/BaseHandle.hpp>

#define IMPL_INTERFACE RTfact::RTpie::IIntersectTask
WRAPPER_START(IntersectTaskHandle, BaseHandle)

    void intersectRays(
        const float* aOrigins,
        const float* aDirections,
        const unsigned int aRayCount,
        float* oDistances)
    IMPLEMENT_WRAPPER_NO_RETURN( IntersectRays(aOrigins, aDirections, aRayCount, oDistances) )

WRAPPER_END
#undef IMPL_INTERFACE



#endif // RTFACT_HANDLE_INTERSECTTASK_HPP
