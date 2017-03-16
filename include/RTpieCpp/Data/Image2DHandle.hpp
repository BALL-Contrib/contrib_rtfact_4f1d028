/* 
 *  Image2DHandle.hpp
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
 
 #ifndef RTFACT_HANDLE_FRAMEBUFFER_HPP
#define RTFACT_HANDLE_FRAMEBUFFER_HPP

#include <RTpieCpp/Config/Common.hpp>

#include <RTpie/Data/IImage2D.hpp>


#define IMPL_INTERFACE RTfact::RTpie::IImage2D
WRAPPER_FWD(Image2DHandle)
#undef IMPL_INTERFACE

#include <RTpieCpp/Data/BufferHandle.hpp>

#define IMPL_INTERFACE RTfact::RTpie::IImage2D
WRAPPER_START(Image2DHandle, BufferHandle)

    typedef RTfact::RTpie::IImage2D::ComponentType ComponentType;

    ComponentType getComponentType()
    IMPLEMENT_WRAPPER_ARG0(ComponentType, GetComponentType)

    int getComponentCount()
    IMPLEMENT_WRAPPER_ARG0(int, GetComponentCount)

    int getWidth()
    IMPLEMENT_WRAPPER_ARG0(int, GetWidth)

    int getHeight()
    IMPLEMENT_WRAPPER_ARG0(int, GetHeight)

WRAPPER_END
#undef IMPL_INTERFACE

RTFACT_HANDLE_NAMESPACE_BEGIN


inline Image2DHandle CreateImage2DHandle(
        Image2DHandle::ComponentType aType,
        unsigned int aComponentCount,
        unsigned int aWidth,
        unsigned int aHeight)
{
    RTfact::RTpie::IImage2D *result;
    RTfact::RTpie::CreateImage2D(aType, aComponentCount, aWidth, aHeight, &result);
    return result;
}

inline Image2DHandle CreateImage2DHandleCopyData(
        Image2DHandle::ComponentType aType,
unsigned int aComponentCount, unsigned int aWidth,
unsigned int aHeight, BufferHandle aData)
{
    RTfact::RTpie::IImage2D *result;
    RTfact::RTpie::CreateImage2DCopyData(aType, aComponentCount, aWidth, aHeight, aData.get(), &result);
    return result;
}

inline Image2DHandle CreateImage2DHandleUseData(
        Image2DHandle::ComponentType aType,
unsigned int aComponentCount, unsigned int aWidth,
unsigned int aHeight, BufferHandle aData)
{
    RTfact::RTpie::IImage2D *result;
    RTfact::RTpie::CreateImage2DUseData(aType, aComponentCount, aWidth, aHeight, aData.get(), &result);
    return result;
}


RTFACT_HANDLE_NAMESPACE_END

#endif // RTFACT_HANDLE_IMAGE_HPP

