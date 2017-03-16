/* 
 *  SDLGLCgWindow.hpp
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
 *  Created on: 2010-06-11 12:01:36 +0200
 *  Author(s): Lukas Marsalek, Petr Kmoch
 */
 
 #ifndef RTFACT__SDLGLCGWINDOW_HPP
#define RTFACT__SDLGLCGWINDOW_HPP

#include <RTfact/Config/Common.hpp>

#include <boost/mpl/size.hpp>

#include <RTfact/Model/Postprocessor/MultiCgFXPostprocessor.hpp>
#include <RTfact/Utils/UI/Windows/SDLGLWindow.hpp>



RTFACT_NAMESPACE_BEGIN

namespace UI {

template <
    typename tEffects>
class SDLGLCgWindow :
    public SDLGLWindow
{
protected:
    uint mTextureX,mTextureY;

public:
    MultiCgFXPostprocessor<tEffects> mPostprocessor;

    void init(
        const uint aResX,
        const uint aResY,
        const bool aFullScreen = false,
        const bool aResizable = true,
        const float aResolutionScale = 1.0f)
    {
        RTFACT_ASSERT(aResolutionScale > 0.f);

        SDLGLWindow::init(
            aResX,
            aResY,
            aFullScreen,
            aResizable);

        {
            int err = glewInit();

            if(GLEW_OK != err)
            {
                RTFACT_PROGRAM_ERROR_F(
                    "Failed to initialize GLEW: %1%", (char*)glewGetErrorString(err));
            }
        }
        
        mTextureX=uint(aResX/aResolutionScale);
        mTextureY=uint(aResY/aResolutionScale);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0.0, mResX, 0.0, mResY);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glViewport(0, 0, mResX, mResY);

        glDisable(GL_BLEND);
        
        std::vector<bool> on(boost::mpl::size<tEffects>::type::value,true);
        mPostprocessor.init(on);
    }
    

    template<
        class tImage2D,
        class tDistanceImage2D,
        class tNormalImage2D>
    void update(
        const tImage2D& aImage,
        const tDistanceImage2D& aDistanceImage,
        const tNormalImage2D& aNormalImage)
    {
        mPostprocessor.fillFBTextures(
            aImage,
            aDistanceImage,
            aNormalImage,
            mTextureX,
            mTextureY);
        mPostprocessor.render(
            mResX,
            mResY,
            mTextureX,
            mTextureY);
    }
    
    template <
        typename tCameraParams>
    void updateCamera(
        const tCameraParams &aParams)
    {
        mPostprocessor.setCameraPosition(
            aParams.position,
            aParams.direction,
            aParams.up);
        mPostprocessor.setCameraOpeningAngle(aParams.openingAngle);
    }
};

} // namespace UI

RTFACT_NAMESPACE_END

#endif // RTFACT__SDLGLCGWINDOW_HPP
