/* 
 *  Postprocessor.hpp
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
 *  Created on: 2010-06-11 12:47:29 +0200
 *  Author(s): Lukas Marsalek
 */
 
 #ifndef RTFACT_REMOTE__POSTPROCESSOR_HPP
#define RTFACT_REMOTE__POSTPROCESSOR_HPP


#include <RTremote/Config/Common.hpp>

#include <GL/glew.h>
#include <Cg/cg.h>
#include <Cg/cgGL.h>

#include <RTremote/Parameterizable.hpp>
#include <RTremote/Effect.hpp>
#include <RTremote/PostprocessorParameter.hpp>

RTFACT_POSTPROCESSING_NAMESPACE_BEGIN

/* PostprocessorPimpl contains the implementation of the postprocessor, hidden from
 * the external application
 */
class PostprocessorPimpl;

class RTFACT_REMOTE_API Postprocessor
{
    PostprocessorPimpl *pimpl;    
    uint mWindowWidth,mWindowHeight;

public:
    /*
     * Buffer types automatically supported through semantic
     */
    struct BufferType
    {
        static const uint COLOR                 = 1;
        static const uint DISTANCE              = (1 << 1);
        static const uint NORMAL                = (1 << 2);
        static const uint AMBIENT_OCCLUSION     = (1 << 3);
        static const uint COLOR_BLEEDING        = (1 << 4);       
    };
        
    Postprocessor();
    
    virtual ~Postprocessor();

    void initEffects();

    void attachBuffer(
        void* aData,
        unsigned int aComponentCount,
        unsigned int aWidth,
        unsigned int aHeight,
        unsigned int aBufferType);

    void setCameraPosition(
        const float aX,
        const float aY,
        const float aZ);

    void setCameraViewDirection(
        const float aX,
        const float aY,
        const float aZ);
    
    void setCameraUpVector(
        const float aX,
        const float aY,
        const float aZ);

    void setCameraOpeningAngle(
        const float aAngle);
    
    uint getOutputWidth() const;
    
    uint getOutputHeight() const;
    
    void setOutputWidth(
        const uint aWidth);
    
    void setOutputHeight(
        const uint aHeight);
    
    void process() const;
    
    GLuint getOutputGLFramebuffer() const;
    
    void setOutputGLFramebuffer(
        const GLuint aGLFramebuffer);
    
    size_t getEffectCount() const;
    
    const EffectHandle getFirstEffect() const;
    
    const EffectHandle getEffectByName(
        const std::string &aName) const;
    
    size_t getParameterCount() const;

    const PostprocessorParameterHandle getFirstParameter() const;
    
    const PostprocessorParameterHandle getParameterByName(
        const std::string &aName) const;

    void setShaderPath(
		    const char* aPath);

    void appendShaderPath(
		    const char* aPath);

    void prependShaderPath(
		    const char* aPath);	
};

RTFACT_POSTPROCESSING_NAMESPACE_END

#endif  //RTREMOTE__POSTPROCESSOR_HPP
