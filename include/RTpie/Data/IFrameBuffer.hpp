/* 
 *  IFrameBuffer.hpp
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
 *  Created on: 2010-09-26 00:35:31 +0200
 *  Author(s): Felix Klein
 */
 
 #ifndef RTFACT_RTPIE__IRENDERBUFFER_HPP
#define RTFACT_RTPIE__IRENDERBUFFER_HPP

#include <RTpie/Config/Common.hpp>
#include <RTpie/Base/IUnknown.hpp>
#include <RTpie/Data/IImage2D.hpp>

RTFACT_RTPIE_NAMESPACE_BEGIN


#define RTPIE_IRENDERBUFFER_ID \
		{ 0x94860e35, 0xda8e, 0x4776, { 0x9a, 0xd5, 0x1, 0xee, 0x8, 0x54, 0xe3, 0x3a } }

/**
 * TODO: Documentation
 */
struct IFrameBuffer : public IUnknown
{

	RTPIE_DECLARE_STATIC_IID_ACCESSOR(RTPIE_IRENDERBUFFER_ID)

    /**
     *  Get the width of the image in pixels.
     *  @param _retval the width of the image
     *  @return HRESULT
     */
    virtual HRESULT GetWidth(int *_retval) = 0;

    /**
     *  Get the height of the image in pixels.
     *  @param _retval the height of the image
     *  @return HRESULT
     */
    virtual HRESULT GetHeight(int *_retval) = 0;

    /**
     *  Set ColorBuffer
     *  @param _retval pointer to Color Buffer Interface
     *  @return HRESULT
     */
    virtual HRESULT SetColorBuffer(IImage2D *_retval) = 0;

    /**
     *  Set DistanceBuffer
     *  @param _retval pointer to Distance Buffer Interface
     *  @return HRESULT
     */
    virtual HRESULT SetDistanceBuffer(IImage2D *_retval) = 0;

};

RTFACT_RTPIE_API HRESULT CreateFrameBuffer(
        IFrameBuffer** _retval);

RTFACT_RTPIE_API HRESULT CreateFrameBuffer(
        IImage2D *aColorBuffer,
        IFrameBuffer** _retval);

RTFACT_RTPIE_NAMESPACE_END

#endif // RTFACT_RTPIE__IMAGE_HPP

