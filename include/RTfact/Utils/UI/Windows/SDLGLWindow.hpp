/*
 *  SDLGLWindow.hpp
 *
 *  RTfact - Real-Time Ray Tracing Library
 *
 *  Copyright (C) 2009  Saarland University
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
 *  Created on: 2008-11-06 02:30:21 +0100
 *  Author(s): Iliyan Georgiev
 */

 #ifndef RTFACT__SDLGLWINDOW_HPP
#define RTFACT__SDLGLWINDOW_HPP

#include <RTfact/Config/Common.hpp>

#include <GL/glew.h>
#include <boost/type_traits/is_same.hpp>

#include <RTfact/Utils/UI/Windows/SDLWindow.hpp>
#include <RTfact/Model/Image/BasicImage2D.hpp>

RTFACT_NAMESPACE_BEGIN

namespace UI {

class SDLGLWindow :
    public SDLWindow
{
    void tonemap(float* tonemapped, size_t size, float gamma, float maxDisplayY) const
    {
        // Compute world adaption luminance
        float Ywa = 0.0f;
        for (unsigned int i=0; i < size / 3u; ++i)
        {
            float lum = 0.2125f * (tonemapped[i*3]) +
                        0.7154f * (tonemapped[i*3+1]) +
                        0.0721f * (tonemapped[i*3+2]);
            if (lum > 0.0f)
                Ywa += logf(lum);
        }
        Ywa = expf(Ywa / (size / 3u));

        // Compute contrast-preserving factor
        float s = powf((1.219f + powf(maxDisplayY / 2.0f, 0.4f)) / (1.219f + powf(Ywa, 0.4f)), 2.5f);

        // Apply scale to all pixels
        float temp = s * 4.0f / maxDisplayY;
        if (fabsf(gamma - 1.0f) < 10E-8)
        {
            // Apply scale factor and do gamma correction
            float inv_gamma = 1.0f / gamma;
            for (unsigned int i=0; i < size; ++i)
            {
                float newVal = powf(temp * (tonemapped[i]), inv_gamma);
                tonemapped[i] = std::max(0.0f,std::min(1.0f, newVal));
            }
        }
        else
        {
            // Apply scale factor without gamma correction
            for (unsigned int i=0; i < size; ++i)
            {
                float newVal = temp * (tonemapped[i]);
                tonemapped[i] = std::max(0.0f,std::min(1.0f, newVal));
            }
        }
    }

    void normalizeDistanceBuffer(float* depthBuffer, size_t size) const
    {
        float dMin = FLT_MAX;
        float dMax1 = 0;
        float dMax2 = 0;

        //find min and two largest values
        //then ignore the larger one => this is the background
        for(uint i = 0; i < size; i++)
        {
            if(depthBuffer[i] < dMin)
            {
                dMin = depthBuffer[i];
            }
            if(depthBuffer[i] > dMax1)
            {
                dMax1 = depthBuffer[i];
            }            
        }

        for(uint i = 0; i < size; i++)
        {
            //When depth buffer is being accumulated, the background value around geometry edges stops to be equal to plain background
            //and is only close to FLT_MAX. But it is still around 10^38. As I do not have any reasonable max distance here
            //(like for example scene bounding box) I have used the constant 100000.f as an approximation. It should ensure
            //that it behaves normally on most scenes.
            if((depthBuffer[i] > dMax2) && (depthBuffer[i] < dMax1) && (depthBuffer[i] < 100000.f))
            {
                dMax2 = depthBuffer[i];
            }
        }
        
        //scale to <0,1>        
        float intervalLengthInv = ((dMax2 - dMin) <= 0) ? 0 : 1.f / (dMax2 - dMin);     
        for(uint i = 0; i < size; i++)
        {
            if(depthBuffer[i] == dMax1)
            {
                depthBuffer[i] = 0.f;
            }
            else
            {
                depthBuffer[i] = (depthBuffer[i] - dMin) * intervalLengthInv;
            }
        }
        //RTFACT_LOG_LINE_F("dMax1, dMax2, dMin: %1%, %2%, %3%", dMax1 % dMax2 % dMin);
    }

    template<
        class tImage2D>
    void drawPixels(
        const GLenum pixelFormat,
        const GLenum channelType,
        const tImage2D& aImage) const
    {
        glDrawPixels(
            aImage.getResX(),
            aImage.getResY(),
            pixelFormat,
            channelType,
            aImage.getFirstComponent(0, 0));
    }

public:

    void init(
        const uint aResX,
        const uint aResY,
        const bool aFullScreen = false,
        const bool aResizable = true,
        const uint aSDLFlags = SDL_OPENGL | SDL_GL_DOUBLEBUFFER)
    {
        uint flags = aSDLFlags;

        if(aFullScreen)
        {
            flags |= SDL_FULLSCREEN;
        }

        if(aResizable)
        {
            flags |= SDL_RESIZABLE;
        }

        SDLWindow::init(aResX, aResY, flags);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    template<
        class tImage2D>
    void update(
        const tImage2D& aImage) const
    {
        RTFACT_ASSERT((
            aImage.getComponentCount() == 1 || aImage.getComponentCount() == 3 || aImage.getComponentCount() == 4));

        RTFACT_STATIC_ASSERT((
            boost::is_same<typename tImage2D::Component, uint8>::value  ||
            boost::is_same<typename tImage2D::Component, float>::value));

        GLenum pixelFormat;
        switch(aImage.getComponentCount())
        {
        case 1:
            pixelFormat = GL_LUMINANCE;
            break;
        case 3:
            pixelFormat = GL_RGB;
            break;
        case 4:
            pixelFormat = GL_RGBA;
            break;
        }
        const GLenum channelType =
            boost::is_same<typename tImage2D::Component, float>::value ?
                GL_FLOAT :
                GL_UNSIGNED_BYTE;

        drawPixels(
            pixelFormat,
            channelType,
            aImage);
    }

    void display() const
    {
        SDL_GL_SwapBuffers();
    }
};

template<>
inline void SDLGLWindow::drawPixels(
    const GLenum pixelFormat,
    const GLenum channelType,
    const RTfact::BasicImage2D<float>& aImage) const
{
    size_t size = aImage.getResX() * aImage.getResY() * aImage.getComponentCount();
    const float *data = aImage.getFirstComponent(0,0);
    float *postProcessed = new float[size];
    memcpy(postProcessed, data, size * sizeof(float));
    if(aImage.getComponentCount() == 1)
    {
        normalizeDistanceBuffer(postProcessed, size);
    }
    else
    {
        tonemap(postProcessed, size, 2.2f, 200.0f);
    }

    glDrawPixels(
        aImage.getResX(),
        aImage.getResY(),
        pixelFormat,
        channelType,
        postProcessed);

    delete[] postProcessed;
}

} // namespace UI

RTFACT_NAMESPACE_END

#endif // RTFACT__SDLGLWINDOW_HPP
