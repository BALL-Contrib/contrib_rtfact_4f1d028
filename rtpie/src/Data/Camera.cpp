/* 
 *  Camera.cpp
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
 *  Created on: 2010-09-26 19:11:26 +0200
 *  Author(s): Felix Klein
 */
 
 #define RTFACT_RTPIE_LIB

#include <Data/Camera.hpp>

RTFACT_RTPIE_NAMESPACE_BEGIN

RTFACT_RTPIE_QI_START(Camera)
RTFACT_RTPIE_QI_ADD(ICamera)
RTFACT_RTPIE_QI_END

RTFACT_RTPIE_API HRESULT CreateCamera(ICamera** _retval)
{
    ICamera* camera = new Camera();
    camera->AddRef();
    *_retval = camera;
    return S_OK;
}

RTFACT_RTPIE_NAMESPACE_END

