/* 
 *  Image2D.hpp
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
 
 #ifndef RTFACT_RTPIE__FRAMEBUFFER_HPP
#define RTFACT_RTPIE__FRAMEBUFFER_HPP

#include "Config/InternalCommon.hpp"

#include "RTpie/Data/IFrameBuffer.hpp"

RTFACT_RTPIE_FORWARD(Image2D)

#include <boost/type_traits/is_same.hpp>
#include <RTfact/Model/Image/BasicImage2D.hpp>

#include "Base/Base.hpp"

#define RTPIE_IMAGE2D_ID \
      { 0xf55a221e, 0x4354, 0x431a, { 0xb3, 0x41, 0x90, 0xd5, 0xec, 0xd9, 0xa5, 0x30 } }



RTFACT_RTPIE_NAMESPACE_BEGIN

class Image2D :
    public Base, public IImage2D
{
    friend class Scene;
    friend class Renderer;

public:

    typedef BasicImage2D<float> t_FloatImage;
    typedef BasicImage2D<uint8> t_ByteImage;

    uint mComponentCount;
    IImage2D::ComponentType mComponentType;
    IBuffer *mBuffer;

public:

    RTPIE_DECLARE_STATIC_IID_ACCESSOR(RTPIE_IMAGE2D_ID)

    // IUnknown

    RTFACT_RTPIE_FORWARD_BASE

    RTFACT_RTPIE_DECLARE_QI

    // IImage2D

    virtual HRESULT GetComponentType(IImage2D::ComponentType *_retval)
    {
        *_retval = mComponentType;
        return S_OK;
    }

    virtual HRESULT GetComponentCount(int *_retval)
    {
        *_retval = mComponentCount;
        return S_OK;
    }

    virtual HRESULT GetWidth(int *_retval)
    {
        if(mComponentType == IImage2D::COMPONENT_FLOAT)
            *_retval = boost::dynamic_pointer_cast<t_FloatImage>(mImage)->getResX();
        else
            *_retval = boost::dynamic_pointer_cast<t_ByteImage>(mImage)->getResX();
        return S_OK;
    }

    virtual HRESULT GetHeight(int *_retval){
        if(mComponentType == IImage2D::COMPONENT_FLOAT)
            *_retval =  boost::dynamic_pointer_cast<t_FloatImage>(mImage)->getResY();
        else
            *_retval =  boost::dynamic_pointer_cast<t_ByteImage>(mImage)->getResY();
        return S_OK;
    }

    // IBuffer

    virtual HRESULT GetData(const void **_retval)
    {
        if(mComponentType == IImage2D::COMPONENT_FLOAT)
            *_retval =  boost::dynamic_pointer_cast<t_FloatImage>(mImage)->getFirstComponent(0,0);
        else
            *_retval =  boost::dynamic_pointer_cast<t_ByteImage>(mImage)->getFirstComponent(0,0);
        return S_OK;
    }

    virtual HRESULT GetDataLength(unsigned int *_retval)
    {
        int width, height;
        GetWidth(&width);
        GetHeight(&height);
        *_retval = width * height * mComponentCount *
            (mComponentType == IImage2D::COMPONENT_FLOAT ? sizeof(float) : sizeof(uint8));
        return S_OK;
    }

    virtual HRESULT StartMap()
    {
        return S_OK;
    }

    virtual HRESULT EndMap()
    {
        return S_OK;
    }

    // Internal

    Traits<RTfact::Image2D>::Ptr mImage;

    explicit Image2D(
        Traits<BasicImage2D<float> >::Ptr& aFrameBuffer
    ) :
        mImage(aFrameBuffer),
        mComponentType(IImage2D::COMPONENT_FLOAT),
        mComponentCount(aFrameBuffer->getComponentCount()),
        mBuffer(0)
    {}

    explicit Image2D(
        Traits<BasicImage2D<uint8> >::Ptr& aFrameBuffer
    ) :
        mImage(aFrameBuffer),
        mComponentType(IImage2D::COMPONENT_BYTE),
        mComponentCount(aFrameBuffer->getComponentCount()),
        mBuffer(0)
    {}

    Image2D(
        IBuffer* aData,
        const IImage2D::ComponentType aComponentType,
        const uint aComponentCount,
        const uint aResX,
        const uint aResY,
        const bool aCopyData = true
    ) :
        mComponentType(aComponentType),
        mComponentCount(aComponentCount),
        mBuffer(0)
    {
        aData->StartMap();
        const void *data;
        aData->GetData(&data);
        aData->EndMap();
        if(!aCopyData)
        {
            mBuffer = aData;
            mBuffer->AddRef();
        }
        if(aComponentType == IImage2D::COMPONENT_FLOAT)
        {
            mImage = new t_FloatImage(
                static_cast<const float*>(data), aComponentCount, aResX, aResY, aCopyData);
        }
        else
        {
            mImage = new t_ByteImage(
                static_cast<const uint8*>(data), aComponentCount, aResX, aResY, aCopyData);
        }
    }

    Image2D(
        const IImage2D::ComponentType aComponentType,
        const uint aComponentCount,
        const uint aResX,
        const uint aResY
    ) :
        mComponentType(aComponentType),
        mComponentCount(aComponentCount),
        mBuffer(0)
    {
        if(aComponentType == IImage2D::COMPONENT_FLOAT)
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

    Traits<RTfact::Image2D>::Ptr getRTfactImage()
    {
        return mImage;
    }

    t_FloatImage* getRTfactFloatImage()
    {
        if(mComponentType != IImage2D::COMPONENT_FLOAT)
            return 0;
        else
            return static_cast<t_FloatImage*>(mImage.get());
    }

    t_ByteImage* getRTfactByteImage()
    {
        if(mComponentType != IImage2D::COMPONENT_BYTE)
            return 0;
        else
            return static_cast<t_ByteImage*>(mImage.get());
    }

    virtual ~Image2D()
    {
        if(mBuffer)
            mBuffer->Release();
    }

};


#define RTFACT_RTPIE_FORWARD_BUFFER2D                            \
    virtual HRESULT GetComponentType(ComponentType *_retval){       \
        return Image2D::GetComponentType(_retval);}             \
    virtual HRESULT GetComponentCount(int *_retval){                \
        return Image2D::GetComponentCount(_retval);}            \
    virtual HRESULT GetWidth(int *_retval){                         \
       return Image2D::GetWidth(_retval);}                      \
    virtual HRESULT GetHeight(int *_retval){                        \
       return Image2D::GetHeight(_retval);}                      \
    virtual HRESULT GetData(void **_retval){                        \
       return Image2D::GetData(_retval);}                       \
    virtual HRESULT GetDataLength(unsigned int *_retval){           \
       return Image2D::GetDataLength(_retval);}


RTFACT_RTPIE_NAMESPACE_END

#endif // RTFACT_RTPIE__IMAGEIMPL_HPP
