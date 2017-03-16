/* 
 *  BufferHandle.hpp
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
 
#ifndef RTFACT_HANDLE_BUFFER_HPP
#define RTFACT_HANDLE_BUFFER_HPP

#include <RTpieCpp/Config/Common.hpp>

#include <RTpie/Data/IBuffer.hpp>


#define IMPL_INTERFACE RTfact::RTpie::IBuffer
WRAPPER_FWD(BufferHandle)
#undef IMPL_INTERFACE

#include <RTpieCpp/Base/BaseHandle.hpp>

#define IMPL_INTERFACE RTfact::RTpie::IBuffer
WRAPPER_START(BufferHandle, BaseHandle)

    const void* getData()
    IMPLEMENT_WRAPPER_ARG0(const void*, GetData)

    unsigned int getDataLength()
    IMPLEMENT_WRAPPER_ARG0(unsigned int, GetDataLength)

WRAPPER_END
#undef IMPL_INTERFACE

RTFACT_HANDLE_NAMESPACE_BEGIN


inline BufferHandle CreateBufferHandle(
        unsigned int aLength)
{
    RTfact::RTpie::IBuffer *result;
    RTfact::RTpie::CreateBuffer(aLength, &result);
    return result;
}

inline BufferHandle CreateBufferHandleUseData(
        unsigned int aLength, const void *aDataSrc)
{
    RTfact::RTpie::IBuffer *result;
    RTfact::RTpie::CreateBufferUseData(aLength, aDataSrc, &result);
    return result;
}

RTFACT_HANDLE_NAMESPACE_END

#endif // RTFACT_HANDLE_IMAGE_HPP

