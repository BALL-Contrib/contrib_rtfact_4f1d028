/*
 *  Image2D.cpp
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
 *  Created on: 2010-09-28 20:24:09 +0200
 *  Author(s): Felix Klein
 */

 #define RTFACT_RTPIE_LIB

#include <Data/Image2D.hpp>

RTFACT_RTPIE_NAMESPACE_BEGIN

RTFACT_RTPIE_QI_START(Image2D)
RTFACT_RTPIE_QI_ADD(IImage2D)
RTFACT_RTPIE_QI_ADD(Image2D)
RTFACT_RTPIE_QI_END

RTPIE_DEFINE_STATIC_IID_ACCESSOR(Image2D, RTPIE_IMAGE2D_ID );


HRESULT CreateImage2D(IImage2D::ComponentType aType,
        unsigned int aComponentCount, unsigned int aWidth,
        unsigned int aHeight, IImage2D** _retval)
{
    IImage2D* image = new Image2D(
        aType, aComponentCount, aWidth, aHeight);
    image->AddRef();
    *_retval = image;
    return S_OK;
}

HRESULT CreateImage2DCopyData(IImage2D::ComponentType aType,
        unsigned int aComponentCount, unsigned int aWidth,
        unsigned int aHeight, IBuffer* aData, IImage2D** _retval)
{
    IImage2D* image = new Image2D(
        aData, aType,
        aComponentCount, aWidth, aHeight, true);
    image->AddRef();
    *_retval = image;
    return S_OK;
}

HRESULT CreateImage2DUseData(IImage2D::ComponentType aType,
        unsigned int aComponentCount, unsigned int aWidth,
        unsigned int aHeight, IBuffer* aData, IImage2D** _retval)
{
    IImage2D* image = new Image2D(
        aData, aType,
        aComponentCount, aWidth, aHeight, false);
    image->AddRef();
    *_retval = image;
    return S_OK;
}

RTFACT_RTPIE_NAMESPACE_END

