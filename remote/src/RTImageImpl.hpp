/* 
 *  RTImageImpl.hpp
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
 *  Created on: 2009-02-27 16:59:53 +0100
 *  Author(s): Iliyan Georgiev, Felix Klein, Petr Kmoch
 */
 
 #ifndef RTFACT_REMOTE__IMAGEIMPL_HPP
#define RTFACT_REMOTE__IMAGEIMPL_HPP

#include <RTremote/Config/Common.hpp>
#include <RTremote/RTImage.hpp>

#include <boost/type_traits/is_same.hpp>

#include <RTfact/Model/Image/BasicImage2D.hpp>

RTFACT_REMOTE_NAMESPACE_BEGIN

class RTImageImpl :
    public RTImage
{
    friend class Renderer;

    typedef BasicImage2D<float> t_FloatImage;
    typedef BasicImage2D<uint8> t_ByteImage;

    uint mComponentCount;
    ComponentType mComponentType;

public:

    Traits<Image2D>::Ptr mImage;

    explicit RTImageImpl(
        Traits<BasicImage2D<float> >::Ptr& aImage
    ) :
        mComponentCount(aImage ? aImage->getComponentCount() : 0),
        mComponentType(COMPONENT_FLOAT),
        mImage(aImage)
    {}

    explicit RTImageImpl(
        Traits<BasicImage2D<uint8> >::Ptr& aImage
    ) :
        mComponentCount(aImage->getComponentCount()),
        mComponentType(COMPONENT_BYTE),
        mImage(aImage)
    {}

    RTImageImpl(
        const void* aData,
        const ComponentType aComponentType,
        const uint aComponentCount,
        const uint aResX,
        const uint aResY,
        const bool aCopyData = true
    ) :
        mComponentCount(aComponentCount),
        mComponentType(aComponentType)
    {
        if(aComponentType == COMPONENT_FLOAT)
        {
            mImage = new t_FloatImage(
                static_cast<const float*>(aData), aComponentCount, aResX, aResY, aCopyData);
        }
        else
        {
            mImage = new t_ByteImage(
                static_cast<const uint8*>(aData), aComponentCount, aResX, aResY, aCopyData);
        }
    }

    RTImageImpl(
        const ComponentType aComponentType,
        const uint aComponentCount,
        const uint aResX,
        const uint aResY
    ) :
        mComponentCount(aComponentCount),
        mComponentType(aComponentType)
    {
        if(aComponentType == COMPONENT_FLOAT)
        {
            mImage = new t_FloatImage(
                aComponentCount, aResX, aResY);
        }
        else
        {
            mImage = new t_ByteImage(
                aComponentCount, aResX, aResY);
        }
    }

    const Traits<Image2D>::Ptr &getRTfactImage() const
    {
        return mImage;
    }

    virtual int getComponentCount() const
    {
        return mComponentCount;
    }

    virtual ComponentType getComponentType() const
    {
        return mComponentType;
    }

    virtual int getWidth() const
    {
        if(mComponentType == COMPONENT_FLOAT)
            return boost::dynamic_pointer_cast<t_FloatImage>(mImage)->getResX();
        else
            return boost::dynamic_pointer_cast<t_ByteImage>(mImage)->getResX();
    }

    virtual int getHeight() const
    {
        if(mComponentType == COMPONENT_FLOAT)
            return boost::dynamic_pointer_cast<t_FloatImage>(mImage)->getResY();
        else
            return boost::dynamic_pointer_cast<t_ByteImage>(mImage)->getResY();
    }

    virtual void* getData()
    {
        if(mComponentType == COMPONENT_FLOAT)
            return boost::dynamic_pointer_cast<t_FloatImage>(mImage)->getFirstComponent(0,0);
        else
            return boost::dynamic_pointer_cast<t_ByteImage>(mImage)->getFirstComponent(0,0);
    }

    virtual size_t getDataLength() const
    {
        return getWidth() * getHeight() * getComponentCount() *
            (getComponentType() == COMPONENT_FLOAT ? sizeof(float) : sizeof(uint8));
    }
};

RTFACT_REMOTE_NAMESPACE_END

#endif // RTFACT_REMOTE__IMAGEIMPL_HPP
