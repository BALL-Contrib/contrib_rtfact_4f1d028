/* 
 *  SDLGLSLWindow.hpp
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
 *  Created on: 2008-11-09 23:28:58 +0100
 *  Author(s): Iliyan Georgiev, Petr Kmoch
 */
 
 #ifndef RTFACT__SDLGLSLWINDOW_HPP
#define RTFACT__SDLGLSLWINDOW_HPP

#include <RTfact/Config/Common.hpp>

#include <fstream>
#include <string>

//#include <GL/glew.h>
//#include <GL/wglew.h>
//#include <SDL_opengl.h>

#include <RTfact/Utils/UI/Windows/SDLGLWindow.hpp>

RTFACT_NAMESPACE_BEGIN

namespace UI {

class SDLGLSLWindow :
    public SDLGLWindow
{
    uint mRealResX;
    uint mRealResY;

    std::string mFragmentProgramFile;
    std::string mFragmentShaderCode;

    //GL_TEXTURE_RECTANGLE_ARB allows non-normalized texture coordinates
    int mTextureTarget;
    int mTextureInternalFormat;
    int mTextureFormat;

    GLuint mFragmentProgram;
    GLuint mFragmentShader;
    GLint mTextureParam, mResXParam, mResYParam;
    GLuint mTextureID;

    int mTextureX;
    int mTextureY;

    bool mFragmentShadersSupported;

    const std::string getGLSLProgram() const
    {
        if (
            !mFragmentShaderCode.empty())
        {
            return mFragmentShaderCode;
        }
        else
        {
            return
                "\
                #extension GL_ARB_texture_rectangle : enable                     \n\
                                                                                   \
                uniform sampler2DRect uTexture;                                    \
                uniform int uResX;                                                 \
                uniform int uResY;                                                 \
                                                                                   \
                vec4 gammaCorrection(float aGamma)                                 \
                {                                                                  \
                    float gammaRecip = 1.0 / aGamma;                               \
                    return pow(texture2DRect(uTexture, gl_TexCoord[0].xy),         \
                               vec4(gammaRecip, gammaRecip, gammaRecip, 1.0));     \
                }                                                                  \
                                                                                   \
                void main()                                                        \
                {                                                                  \
                    gl_FragColor = gammaCorrection(1.4);                           \
                }                                                                  \
                ";
        }
    }

    const std::string loadTextFile(
        const std::string& aFileName) const
    {
        std::ifstream input(aFileName.c_str());

        RTFACT_USER_ERROR_IF_F(!input, "Could not open file: %1%", aFileName);

        std::string retval;

        while(!input.eof())
        {
            std::string buf;
            getline(input, buf);
            buf.append("\n");
            retval.append(buf);
        }

        return retval;
    }

    void printProgramInfoLog(GLuint obj)
    {
        int infologLength = 0;
        int charsWritten  = 0;
        char *infoLog;

        glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
        
        if(infologLength > 1)
        {
            infoLog = (char*) malloc(infologLength);
            glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
            
            RTFACT_PROGRAM_WARNING(infoLog);

            free(infoLog);
        }
    }

    void printShaderInfoLog(GLuint obj)
    {
        int infologLength = 0;
        int charsWritten  = 0;
        char *infoLog;

        glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

        if(infologLength > 1) 
        {
            infoLog = (char*) malloc(infologLength);
            glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
            
            RTFACT_PROGRAM_WARNING(infoLog);

            free(infoLog);
        }
    }

    void cleanup()
    {
        //texture
        glDeleteTextures(1, &mTextureID);
        //shader
        glDetachShader(mFragmentProgram, mFragmentShader);
        glDeleteShader(mFragmentShader); 
        glDeleteProgram(mFragmentProgram); 
    }

public:

    void loadFragmentProgram(
        const std::string &aFSFile)
    {
        mFragmentShaderCode=loadTextFile(aFSFile);
    }
    
    void init(
        const uint aResX,
        const uint aResY,
        const bool aFullScreen = false,
        const bool aResizable = true,
        const float aResolutionScale = 1.0f)
    {
        RTFACT_ASSERT(aResolutionScale > 0.f);

        mRealResX = aResX;
        mRealResY = aResY;

        SDLGLWindow::init(
            static_cast<uint>(aResX * aResolutionScale),
            static_cast<uint>(aResY * aResolutionScale),
            aFullScreen, aResizable);

        mTextureX = static_cast<int>(mRealResX); // * 3;
        mTextureY = static_cast<int>(mRealResY);

        int err = glewInit();

        if(GLEW_OK != err)
        {
            RTFACT_PROGRAM_ERROR_F(
                "Failed to initialize GLEW: %1%", (char*)glewGetErrorString(err));
        }

        mFragmentShadersSupported = true;

        if(!GLEW_ARB_fragment_shader)
        {
            mFragmentShadersSupported = false;

            RTFACT_PROGRAM_WARNING(
                "Fragment shaders not supported. Disabling GPU postprocessing.");

            return;
        }

        mTextureTarget = GL_TEXTURE_RECTANGLE_ARB;

        // create program object
        mFragmentProgram = glCreateProgram();

        // create shader object (fragment shader)
        mFragmentShader = glCreateShader(GL_FRAGMENT_SHADER_ARB);

        // set source for shader
        std::string shader = getGLSLProgram();
        const char* source = shader.c_str();
        //RTFACT_LOG_LINE(getGLSLProgram().c_str());
        glShaderSource(mFragmentShader, 1, &source, RTFACT_NULL);

        // compile shader
        glCompileShader(mFragmentShader);

        // check for errors
        printShaderInfoLog(mFragmentShader);

        // attach shader to program
        glAttachShader(mFragmentProgram, mFragmentShader);

        // link into full program, use fixed function vertex pipeline
        glLinkProgram(mFragmentProgram);

        // check for errors
        printProgramInfoLog(mFragmentProgram);

        // Get the locations of the uniform variables
        mTextureParam = glGetUniformLocation(mFragmentProgram, "uTexture");
        mResXParam = glGetUniformLocation(mFragmentProgram, "uResX");
        mResYParam = glGetUniformLocation(mFragmentProgram, "uResY");

        // create a new texture name
        glGenTextures (1, &mTextureID);

        // bind the texture name to a texture target
        glBindTexture(mTextureTarget, mTextureID);

        // turn off filtering and set proper wrap mode 
        // (obligatory for float textures atm)
        glTexParameteri(mTextureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(mTextureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(mTextureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(mTextureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP);

        // set texenv to replace instead of the default modulate
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

        // viewport for 1:1 pixel=texel=geometry mapping
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0.0, mResX, 0.0, mResY);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glViewport(0, 0, mResX, mResY);
    }

    template<
        class tImage2D>
    void update(
        const tImage2D& aImage)
    {
        RTFACT_STATIC_ASSERT((
            boost::is_same<typename tImage2D::Component, uint8>::value  ||
            boost::is_same<typename tImage2D::Component, float>::value));

        RTFACT_ASSERT((
            aImage.getComponentCount() == 3 || aImage.getComponentCount() == 4));

        if (
            aImage.getComponentCount() == 4)
        {
            //if alpha is used, image must be cleared for blending to work correctly
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
        }

        const GLenum pixelFormat =
            aImage.getComponentCount() == 4 ?
                GL_RGBA :
                GL_RGB;

        const GLenum channelType =
            boost::is_same<typename tImage2D::Component, float>::value ?
                GL_FLOAT :
                GL_UNSIGNED_BYTE;

        // enable texture x (read-only, not changed in the computation loop)
        glActiveTexture(GL_TEXTURE0);    
        glBindTexture(mTextureTarget, mTextureID);

        if(mFragmentShadersSupported)
        {
            // enable GLSL program
            glUseProgram(mFragmentProgram);

            glUniform1i(mTextureParam, 0); // texunit 0
            glUniform1i(mResXParam, mTextureX);
            glUniform1i(mResYParam, mTextureY);
        }

        //timing purposes only
        //glFinish();

        ////////////////////////////////////////////////////////////////////////
        //NVidia
        glEnable(mTextureTarget);

        glTexImage2D(
            mTextureTarget, 
            0, //not to use any mipmap levels for this texture
            pixelFormat,
            mTextureX,
            mTextureY,
            0, //turns off borders
            pixelFormat,
            channelType,
            0);

        glTexSubImage2D(
            mTextureTarget,
            0, 0, 0,
            mTextureX, mTextureY,
            pixelFormat,
            channelType,
            aImage.getFirstComponent(0,0));
        ////////////////////////////////////////////////////////////////////////

        // make quad filled to hit every pixel/texel
        glPolygonMode(GL_FRONT,GL_FILL);
        // and render quad
        glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0); 
        glVertex2f(0.0, 0.0);
        glTexCoord2f(static_cast<float>(mTextureX), 0.0); 
        glVertex2f(static_cast<float>(mResX), 0.0);
        glTexCoord2f(static_cast<float>(mTextureX), static_cast<float>(mTextureY)); 
        glVertex2f(static_cast<float>(mResX), static_cast<float>(mResY));
        glTexCoord2f(0.0, static_cast<float>(mTextureY)); 
        glVertex2f(0.0, static_cast<float>(mResY));
        glEnd();

        //un-bind texture
        glBindTexture(mTextureTarget, 0);

        glDisable(mTextureTarget);
    }

    uint getResX() const { return mRealResX; }

    uint getResY() const { return mRealResY; }
};

} // namespace UI

RTFACT_NAMESPACE_END

#endif // RTFACT__SDLGLSLWINDOW_HPP
