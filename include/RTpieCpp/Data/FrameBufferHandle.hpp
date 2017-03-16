/* 
 *  FrameBufferHandle.hpp
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
 *  Created on: 2010-09-26 18:55:36 +0200
 *  Author(s): Felix Klein
 */
 
 #ifndef RTFACT_HANDLE_RENDERBUFFER_HPP
#define RTFACT_HANDLE_RENDERBUFFER_HPP

#include <RTpieCpp/Config/Common.hpp>

#include <RTpie/Data/IFrameBuffer.hpp>


#define IMPL_INTERFACE RTfact::RTpie::IFrameBuffer
WRAPPER_FWD(FrameBufferHandle)
#undef IMPL_INTERFACE

#include <RTpieCpp/Base/BaseHandle.hpp>
#include <RTpieCpp/Data/Image2DHandle.hpp>

#define IMPL_INTERFACE RTfact::RTpie::IFrameBuffer
WRAPPER_START(FrameBufferHandle, BaseHandle)

    int getWidth()
    IMPLEMENT_WRAPPER_ARG0(int, GetWidth)

    int getHeight()
    IMPLEMENT_WRAPPER_ARG0(int, GetHeight)

    void setColorBuffer(Image2DHandle aBuffer)
    IMPLEMENT_WRAPPER_NO_RETURN( SetColorBuffer(aBuffer.get()) )

    void setDistanceBuffer(Image2DHandle aBuffer)
    IMPLEMENT_WRAPPER_NO_RETURN( SetDistanceBuffer(aBuffer.get()) )


WRAPPER_END
#undef IMPL_INTERFACE


RTFACT_HANDLE_NAMESPACE_BEGIN

inline FrameBufferHandle CreateFrameBufferHandle()
{
    RTfact::RTpie::IFrameBuffer *result;
    RTfact::RTpie::CreateFrameBuffer(&result);
    return result;
}

inline FrameBufferHandle CreateFrameBufferHandle(Image2DHandle aBuffer)
{
    RTfact::RTpie::IFrameBuffer *result;
    RTfact::RTpie::CreateFrameBuffer(aBuffer.get(), &result);
    return result;
}


RTFACT_HANDLE_NAMESPACE_END

#endif // RTFACT_HANDLE_IMAGE_HPP

