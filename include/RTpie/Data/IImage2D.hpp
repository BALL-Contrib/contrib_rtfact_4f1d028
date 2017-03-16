/* 
 *  IImage2D.hpp
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
 
 #ifndef RTFACT_RTPIE__IIMAGE2D_HPP
#define RTFACT_RTPIE__IIMAGE2D_HPP

#include <RTpie/Config/Common.hpp>
#include <RTpie/Data/IBuffer.hpp>

RTFACT_RTPIE_NAMESPACE_BEGIN


#define RTPIE_IIMAGE_ID \
		{ 0x94860e35, 0xda8e, 0x4776, { 0x9a, 0xd5, 0x1, 0xee, 0x8, 0x54, 0xe3, 0x3a } }

/**
 * TODO: Documentation
 */
struct IImage2D : public IBuffer
{

	RTPIE_DECLARE_STATIC_IID_ACCESSOR(RTPIE_IIMAGE_ID)

    enum ComponentType
    {
        COMPONENT_FLOAT,
        COMPONENT_BYTE
    };

    /**
     *  Get ComponentType of Image.
     *  This is either IImage2D::COMPONENT_FLOAT or IImage2D::COMPONENT_BYTE
     *  @param _retval the component type of the image
     *  @return HRESULT
     */
    virtual HRESULT GetComponentType(ComponentType *_retval) = 0;

    /**
     *  Get number of components per pixel. This is usually 3 for RGB or 4 for RGBA.
     *  @param _retval the component count of the image
     *  @return HRESULT
     */
    virtual HRESULT GetComponentCount(int *_retval) = 0;

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

};

/**
 *  Create an image with the given attributes
 *  @param aType Type of the image Components. Must be either
 *  IImage2D::COMPONENT_FLOAT or IImage2D::COMPONENT_BYTE
 *  @param aComponentCount Number of components per pixel
 *  @param aWidth width of the image in pixels
 *  @param aHeight height of the image in pixels
 *  @param _retval the IImage2D interface pointer of the newly created image
 *  @return HRESULT
 */
RTFACT_RTPIE_API HRESULT CreateImage2D(IImage2D::ComponentType aType,
        unsigned int aComponentCount, unsigned int aWidth,
        unsigned int aHeight, IImage2D** _retval);


/**
 *  Create an image with the given attributes
 *  the Data provided in this function is COPIED
 *  @param aType Type of the image Components. Must be either
 *  IImage2D::COMPONENT_FLOAT or IImage2D::COMPONENT_BYTE
 *  @param aComponentCount Number of components per pixel
 *  @param aWidth width of the image in pixels
 *  @param aHeight height of the image in pixels
 *  @param aData the pixel data of the image. The data is copied.
 *  @param _retval the IImage2D interface pointer of the newly created image
 *  @return HRESULT
 */
RTFACT_RTPIE_API HRESULT CreateImage2DCopyData(IImage2D::ComponentType aType,
        unsigned int aComponentCount, unsigned int aWidth,
        unsigned int aHeight, IBuffer* aData, IImage2D** _retval);

/**
 *  Create an image with the given attributes
 *  the Data provided in this function is DIRECTLY USED
 *  Make sure that the data is NOT FREED while this image is active
 *  @param aType Type of the image Components. Must be either
 *  IImage2D::COMPONENT_FLOAT or IImage2D::COMPONENT_BYTE
 *  @param aComponentCount Number of components per pixel
 *  @param aWidth width of the image in pixels
 *  @param aHeight height of the image in pixels
 *  @param aData the pixel data of the image. The data is copied.
 *  @param _retval the IImage2D interface pointer of the newly created image
 *  @return HRESULT
 */
RTFACT_RTPIE_API HRESULT CreateImage2DUseData(IImage2D::ComponentType aType,
        unsigned int aComponentCount, unsigned int aWidth,
        unsigned int aHeight, IBuffer* aData, IImage2D** _retval);



RTFACT_RTPIE_NAMESPACE_END

#endif // RTFACT_RTPIE__IIMAGE2D_HPP

