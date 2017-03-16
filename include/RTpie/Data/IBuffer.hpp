/* 
 *  IBuffer.hpp
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
 
 #ifndef RTFACT_RTPIE__IBUFFER_HPP
#define RTFACT_RTPIE__IBUFFER_HPP

#include <RTpie/Config/Common.hpp>
#include <RTpie/Base/IUnknown.hpp>

RTFACT_RTPIE_NAMESPACE_BEGIN


#define RTPIE_IBUFFER_ID \
		{ 0x72758e32, 0x6a34, 0x491a, { 0xa1, 0x8c, 0x24, 0x3d, 0x4, 0x31, 0xac, 0x18 } }

/**
 * TODO: Documentation
 */
struct IBuffer : public IUnknown
{

	RTPIE_DECLARE_STATIC_IID_ACCESSOR(RTPIE_IBUFFER_ID)

    enum ComponentType
    {
        COMPONENT_FLOAT,
        COMPONENT_BYTE
    };

    /**
     *  Get the data pointer of the buffer.
     *  @param _retval the data pointer of the image
     *  @return HRESULT
     */
    virtual HRESULT GetData(const void **_retval) = 0;

    /**
     *  Get the length in bytes of the data allocated for this buffer.
     *  @param _retval the length of the image data
     *  @return HRESULT
     */
    virtual HRESULT GetDataLength(unsigned int *_retval) = 0;

    /**
     *  Prepare the data for access. This function must be called before the data is accessed.
     *  @return HRESULT
     */
    virtual HRESULT StartMap() = 0;

    /**
     *  Notify that data access is finished. This function must be called after the data is accessed.
     *  @return HRESULT
     */
    virtual HRESULT EndMap() = 0;
};


RTFACT_RTPIE_API HRESULT CreateBufferUseData(
        unsigned int length,
        const void* aDataSrc,
        IBuffer** _retval);

RTFACT_RTPIE_API HRESULT CreateBuffer(
        unsigned int length,
        IBuffer** _retval);

RTFACT_RTPIE_NAMESPACE_END

#endif // RTFACT_RTPIE__IMAGE_HPP

