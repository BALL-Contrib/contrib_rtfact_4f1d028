/* 
 *  SDLGLPostprocessorWindow.hpp
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
 *  Created on: 2010-06-11 12:53:39 +0200
 *  Author(s): Lukas Marsalek, Petr Kmoch
 */
 
 #ifndef RTFACT__SDLGLPOSTPROCESSORWINDOW_HPP
#define RTFACT__SDLGLPOSTPROCESSORWINDOW_HPP

#include <RTfact/Config/Common.hpp>

#include <boost/functional.hpp>
#include <RTfact/Utils/UI/Windows/SDLGLWindow.hpp>



RTFACT_NAMESPACE_BEGIN

namespace UI {

template <
    typename tPostprocessor>
class SDLGLPostprocessorWindow :
    public SDLGLWindow
{
protected:
    tPostprocessor *mPostprocessor;
    
    void resizePostprocessor() const
    {
        if (
            mPostprocessor!=NULL)
        {
            mPostprocessor->setOutputWidth(mResX);
            mPostprocessor->setOutputHeight(mResY);
        }
    }
    
    void newSize() const
    {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0.0, GLdouble(mResX), 0.0, GLdouble(mResY));
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glViewport(0, 0, mResX, mResY);
        resizePostprocessor();
    }
    
    void resize(
        const Event &aEvent)
    {
        if (
            aEvent.type==Event::Resize)
        {
            mResX=aEvent.resolution.x;
            mResY=aEvent.resolution.y;
            newSize();
        }
    }

public:
    SDLGLPostprocessorWindow() :
        SDLGLWindow(),
        mPostprocessor(NULL)
    {}
    
    void init(
        const uint aResX,
        const uint aResY,
        const bool aFullScreen = false,
        const bool aResizable = true)
    {
        SDLGLWindow::init(
            aResX,
            aResY,
            aFullScreen,
            aResizable);

        {
            GLenum err = glewInit();

            if (
                err != GLEW_NO_ERROR)
            {
                RTFACT_PROGRAM_ERROR_F(
                    "Failed to initialize GLEW: %1%", (char*)glewGetErrorString(err));
            }
        }

        newSize();

        glDisable(GL_BLEND);
        
        registerEventHandler(boost::bind1st(boost::mem_fun(&SDLGLPostprocessorWindow::resize),this));
    }
    
    const tPostprocessor * const getPostprocessor() const
    {
        return mPostprocessor;
    }
    
    tPostprocessor * const getPostprocessor()
    {
        return mPostprocessor;
    }
    
    void setPostprocessor(
        tPostprocessor * const aPostprocessor)
    {
        mPostprocessor=aPostprocessor;
        resizePostprocessor();
    }
};

} // namespace UI

RTFACT_NAMESPACE_END

#endif // RTFACT__SDLGLPOSTPROCESSORWINDOW_HPP
