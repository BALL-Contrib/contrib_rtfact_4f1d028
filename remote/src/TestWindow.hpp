/* 
 *  TestWindow.hpp
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
 *  Created on: 2009-09-30 14:15:34 +0200
 *  Author(s): Felix Klein
 */
 
 #ifndef RTFACT__SDLGLSLWINDOW_HPP
#define RTFACT__SDLGLSLWINDOW_HPP

#include <RTremote/Config/Common.hpp>

#include <fstream>
#include <string>

#include <GL/glew.h>
#include <SDL.h>
//#include <GL/wglew.h>
//#include <SDL_opengl.h>

RTFACT_REMOTE_NAMESPACE_BEGIN

class TestWindow
{

    enum
    {
        COLOR_DEPTH = 32
    };

    uint mRealResX;
    uint mRealResY;
    uint mResX;
    uint mResY;

    std::string mFragmentProgramFile;

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

    const std::string loadTextFile(
        const std::string& aFileName) const
    {
        std::ifstream input(aFileName.c_str());

        //RTFACT_USER_ERROR_IF_F(!input, "Could not open file: %1%", aFileName);

        std::string retval;

        while(!input.eof())
        {
            std::string buf;
            getline(input, buf);
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

            //RTFACT_PROGRAM_WARNING(infoLog);

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

            //RTFACT_PROGRAM_WARNING(infoLog);

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

    void init(
        const uint aResX,
        const uint aResY,
        const bool aFullScreen = false,
        const bool aResizable = true,
        const float aResolutionScale = 1.0f)
    {
        //RTFACT_ASSERT(aResolutionScale > 0.f);

        mRealResX = aResX;
        mRealResY = aResY;


        uint flags = SDL_OPENGL | SDL_GL_DOUBLEBUFFER;

        if(aFullScreen)
        {
            flags |= SDL_FULLSCREEN;
        }

        if(aResizable)
        {
            flags |= SDL_RESIZABLE;
        }


        mResX = aResX * aResolutionScale;
        mResY = aResY * aResolutionScale;

        SDL_SetVideoMode(mResX, mResY, COLOR_DEPTH, flags);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        mTextureX = static_cast<int>(mRealResX); // * 3;
        mTextureY = static_cast<int>(mRealResY);

        int err = glewInit();

        if(GLEW_OK != err)
        {
            //RTFACT_PROGRAM_ERROR_F(
            //    "Failed to initialize GLEW: %1%", (char*)glewGetErrorString(err));
        }

        mFragmentShadersSupported = true;

        if(!GLEW_ARB_fragment_shader)
        {
            mFragmentShadersSupported = false;

            //RTFACT_PROGRAM_WARNING(
            //    "Fragment shaders not supported. Disabling GPU postprocessing.");

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
        glShaderSource(mFragmentShader, 1, &source, 0);

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
        class tComponent>
    void update(
        tComponent* data, uint8 aComponentCount)
    {
        //RTFACT_STATIC_ASSERT((
        //    boost::is_same<typename tComponent, uint8>::value  ||
        //    boost::is_same<typename tComponent, float>::value));

        //RTFACT_ASSERT((
        //    aComponentCount == 3 || aComponentCount == 4));

        const GLenum pixelFormat =
            aComponentCount == 4 ?
                GL_RGBA :
                GL_RGB;

        const GLenum channelType =
            boost::is_same<typename tComponent, float>::value ?
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
            data);
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


    void display() const
    {
        SDL_GL_SwapBuffers();
    }
};

RTFACT_REMOTE_NAMESPACE_END

#endif // RTFACT__SDLGLSLWINDOW_HPP
