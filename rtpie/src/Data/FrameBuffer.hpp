/*
 *  FrameBuffer.hpp
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

 #ifndef RTFACT_RTPIE__RENDERBUFFER_HPP
#define RTFACT_RTPIE__RENDERBUFFER_HPP

#include "Config/InternalCommon.hpp"

#include <RTpie/Data/IFrameBuffer.hpp>

RTFACT_RTPIE_FORWARD(FrameBuffer)

#include "Base/Base.hpp"
#include "Data/Image2D.hpp"

RTFACT_RTPIE_NAMESPACE_BEGIN

class FrameBuffer :
    public Base, public IFrameBuffer
{


public:

    // IUnknown

    RTFACT_RTPIE_DECLARE_QI

    RTFACT_RTPIE_FORWARD_BASE

    // IRenderBuffer

    virtual HRESULT GetWidth(int *_retval)
    {
        *_retval =  mWidth;
        return S_OK;
    }

    virtual HRESULT GetHeight(int *_retval)
    {
        *_retval =  mHeight;
        return S_OK;
    }

    virtual HRESULT SetColorBuffer(IImage2D *aBuffer)
    {

        void *p = 0;
        aBuffer->QueryInterface(Image2D::GetIID(), &p);
        if(p)
        {
            Image2DPtr fb = static_cast<Image2D*>(p);
            fb->Release();
            // TODO: Check Buffer Format

            if(!checkSize(fb))
                return E_FAIL;

            mColorBuffer = fb;
            return S_OK;
        }
        return E_FAIL;
    }

    virtual HRESULT SetDistanceBuffer(IImage2D *aBuffer)
    {
        void *p;
        aBuffer->QueryInterface(Image2D::GetIID(), &p);
        if(p)
        {
            Image2DPtr fb = static_cast<Image2D*>(p);
            fb->Release();

            if(fb->mComponentType != IImage2D::COMPONENT_FLOAT ||
               fb->mComponentCount != 1)
                return E_FAIL;

            if(!checkSize(fb))
                return E_FAIL;
            // TODO: Check Buffer Format
            mDistanceBuffer = fb;

            return S_OK;
        }
        return E_FAIL;

    }

    // Internal

    FrameBuffer() : mWidth(-1), mHeight(-1)
    {}

    virtual ~FrameBuffer(){}


    FrameBuffer(IImage2D *aColorBuffer)
    {
        aColorBuffer->GetWidth(&mWidth);
        aColorBuffer->GetHeight(&mHeight);
        SetColorBuffer(aColorBuffer);
    }

    bool checkSize(Image2DPtr aBuffer)
    {
        if(mWidth == -1){
            aBuffer->GetWidth(&mWidth);
            aBuffer->GetHeight(&mHeight);
            return true;
        }
        int width, height;
        aBuffer->GetWidth(&width);
        aBuffer->GetHeight(&height);
        if(width != mWidth || height != mHeight)
            return false;
        else
            return true;
    }

    bool hasByteRenderBuffer(){
        return mColorBuffer->mComponentType == IImage2D::COMPONENT_BYTE;
    }

    bool getRTfactByteRenderBuffer(t_RTpieByteFramebuffer &aBuffer)
    {
        if(!hasByteRenderBuffer())
            return false;
        if(mColorBuffer)
            aBuffer.setColorImage(mColorBuffer->getRTfactByteImage());
        if(mDistanceBuffer)
            aBuffer.setDistanceImage(mDistanceBuffer->getRTfactFloatImage());
        return true;
    }

    bool getRTfactFloatRenderBuffer(t_RTpieFloatFramebuffer &aBuffer)
    {
        if(hasByteRenderBuffer())
            return false;
        if(mColorBuffer)
            aBuffer.setColorImage(mColorBuffer->getRTfactFloatImage());
        if(mDistanceBuffer)
            aBuffer.setDistanceImage(mDistanceBuffer->getRTfactFloatImage());
        return true;
    }

    int getWidth(){ return mWidth; }
    int getHeight(){ return mHeight; }



    int mWidth, mHeight;
    Image2DPtr mColorBuffer,
    mDistanceBuffer;

};


RTFACT_RTPIE_NAMESPACE_END

#endif // RTFACT_RTPIE__IMAGEIMPL_HPP
