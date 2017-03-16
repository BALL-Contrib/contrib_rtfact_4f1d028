/* 
 *  Postprocessor.cpp
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
 *  Author(s): Lukas Marsalek, Petr Kmoch, Felix Klein
 */
 
 #include "Precompiled.hpp"

#include <RTfactRemoteConfig.hpp>
#include <RTfact/Config/Common.hpp>

#define RTFACT_REMOTE_LIB

#include <boost/mpl/for_each.hpp>

#include <RTremote/Postprocessor.hpp>
#include <RTremote/Renderer.hpp>

#include "RTPrivatePostprocessor.hpp"

RTFACT_REMOTE_NAMESPACE_BEGIN

//---------------------------------------------------------------------
// Postprocessor Definition
//---------------------------------------------------------------------

Postprocessor::Postprocessor() :
    pimpl(new PostprocessorPimpl())    
{
}

Postprocessor::~Postprocessor()
{
    delete pimpl;
}

void Postprocessor::initEffects()
{
	pimpl->initEffects();
}

void Postprocessor::attachBuffer(
        void* aData,
        unsigned int aComponentCount,
        unsigned int aWidth,
        unsigned int aHeight,
        unsigned int aBufferType)
{
    if(aData == NULL)
    {
        return;
    }

    if(RTFACT_CHECK_UMASK(aBufferType, Postprocessor::BufferType::COLOR))
    {
        pimpl->mColorImagePtr = new t_ColorImage(
        reinterpret_cast<t_ColorImageComponent*>(aData),
        aComponentCount, aWidth, aHeight, false);
    }
    else if(RTFACT_CHECK_UMASK(aBufferType, Postprocessor::BufferType::DISTANCE))
    {
        pimpl->mDistanceImagePtr = new t_DistanceImage(
        reinterpret_cast<t_DistanceImageComponent*>(aData),
        aComponentCount, aWidth, aHeight, false);
    }
    else if(RTFACT_CHECK_UMASK(aBufferType, Postprocessor::BufferType::NORMAL))
    {
        pimpl->mNormalImagePtr = new t_NormalImage(
        reinterpret_cast<t_NormalImageComponent*>(aData),
        aComponentCount, aWidth, aHeight, false);
    }
    else if(RTFACT_CHECK_UMASK(aBufferType, Postprocessor::BufferType::AMBIENT_OCCLUSION))
    {
        pimpl->mAmbientOcclusionImagePtr = new t_AmbientOcclusionImage(
        reinterpret_cast<t_AmbientOcclusionImageComponent*>(aData),
        aComponentCount, aWidth, aHeight, false);
    }
    else if(RTFACT_CHECK_UMASK(aBufferType, Postprocessor::BufferType::COLOR_BLEEDING))
    {
        pimpl->mColorBleedingImagePtr = new t_ColorBleedingImage(
        reinterpret_cast<t_ColorBleedingImageComponent*>(aData),
        aComponentCount, aWidth, aHeight, false);
    }
    else
    {
        RTFACT_PROGRAM_WARNING("Unknown buffer type specified in Postprocessor::attachBuffer");
        return;
    }
}

void Postprocessor::setCameraPosition(
        const float aX,
        const float aY,
        const float aZ)
{
    pimpl->mCameraPosition.setXf(aX);
    pimpl->mCameraPosition.setYf(aY);
    pimpl->mCameraPosition.setZf(aZ);
}

void Postprocessor::setCameraViewDirection(
    const float aX,
    const float aY,
    const float aZ)
{
    pimpl->mCameraViewDirection.setXf(aX);
    pimpl->mCameraViewDirection.setYf(aY);
    pimpl->mCameraViewDirection.setZf(aZ);
}

void Postprocessor::setCameraUpVector(
    const float aX,
    const float aY,
    const float aZ)
{
    pimpl->mCameraUpVector.setXf(aX);
    pimpl->mCameraUpVector.setYf(aY);
    pimpl->mCameraUpVector.setZf(aZ);
}

void Postprocessor::setCameraOpeningAngle(
    const float aAngle)
{
    pimpl->mCameraOpeningAngle = aAngle;
}

uint Postprocessor::getOutputWidth() const
{
    return mWindowWidth;
}

uint Postprocessor::getOutputHeight() const
{
    return mWindowHeight;
}

void Postprocessor::setOutputWidth(
    const uint aWidth)
{
    mWindowWidth=aWidth;
}

void Postprocessor::setOutputHeight(
    const uint aHeight)
{
    mWindowHeight=aHeight;
}

void Postprocessor::process() const
{
    //ToDo: All buffers should be checked
    RTFACT_ASSERT(pimpl->mColorImagePtr != NULL);    

    //ToDo: think of a way to optimize unnecessary re-setting of unchanged values
    pimpl->mPostprocessor.setCameraPosition(
            pimpl->mCameraPosition,
            pimpl->mCameraViewDirection,
            pimpl->mCameraUpVector);
    pimpl->mPostprocessor.setCameraOpeningAngle(pimpl->mCameraOpeningAngle);
  
    pimpl->mPostprocessor.fillFBTextures(
        *(pimpl->mColorImagePtr),
        *(pimpl->mDistanceImagePtr),
        *(pimpl->mNormalImagePtr),
		*(pimpl->mAmbientOcclusionImagePtr),
        *(pimpl->mColorBleedingImagePtr),
        pimpl->mColorImagePtr->getResX(),
        pimpl->mColorImagePtr->getResY());

    pimpl->mPostprocessor.render(
        mWindowWidth,
        mWindowHeight,        
        pimpl->mColorImagePtr->getResX(),
        pimpl->mColorImagePtr->getResY());
}

GLuint Postprocessor::getOutputGLFramebuffer() const
{
    return pimpl->mPostprocessor.getOutputGLFramebuffer();
}

void Postprocessor::setOutputGLFramebuffer(
    const GLuint aGLFramebuffer)
{
    pimpl->mPostprocessor.setOutputGLFramebuffer(aGLFramebuffer);
}

size_t Postprocessor::getEffectCount() const
{
    return pimpl->mEffects.size();
}

const EffectHandle Postprocessor::getFirstEffect() const
{
    if (
        pimpl->mEffects.empty())
    {
        return NULL;
    }
    else
    {
        return pimpl->mEffects.front();
    }
}

struct getEffectByName_hasName :
    public std::unary_function<EffectHandle,const bool>
{
    std::string mName;
    getEffectByName_hasName(const std::string &aName) : mName(aName) {}

    bool operator() (
        EffectHandle aHandle) const
    {
        return aHandle->getName()==mName;
    }
};

const EffectHandle Postprocessor::getEffectByName(
    const std::string &aName) const
{
    
    std::vector<EffectHandle>::const_iterator it=std::find_if(
        pimpl->mEffects.begin(),
        pimpl->mEffects.end(),
        getEffectByName_hasName(aName));
    
    if (
        it!=pimpl->mEffects.end())
    {
        return *it;
    }
    else
    {
        return NULL;
    }
}

size_t Postprocessor::getParameterCount() const
{
    return pimpl->mParameters.size();
}

const PostprocessorParameterHandle Postprocessor::getFirstParameter() const
{
    if (
        pimpl->mParameters.empty())
    {
        return NULL;
    }
    else
    {
        return pimpl->mParameters.front();
    }
}

struct getParameterByName_hasName :
    public std::unary_function<PostprocessorParameterHandle,const bool>
{
    std::string mName;
    getParameterByName_hasName(const std::string &aName) : mName(aName) {}

    bool operator() (
        PostprocessorParameterHandle aHandle) const
    {
        return aHandle->getName()==mName;
    }
};

const PostprocessorParameterHandle Postprocessor::getParameterByName(
    const std::string &aName) const
{
    std::vector<PostprocessorParameterHandle>::const_iterator it=std::find_if(
        pimpl->mParameters.begin(),
        pimpl->mParameters.end(),
        getParameterByName_hasName(aName));
    
    if (
        it!=pimpl->mParameters.end())
    {
        return *it;
    }
    else
    {
        return NULL;
    }
}

void Postprocessor::setShaderPath(const char *aPath)
{
	const std::string strPath(aPath);
	pimpl->mPostprocessor.clearShaderPaths();
	pimpl->mPostprocessor.appendShaderPath(strPath);		
}

void Postprocessor::appendShaderPath(const char *aPath)
{
	const std::string strPath(aPath);
	pimpl->mPostprocessor.appendShaderPath(strPath);	
}

void Postprocessor::prependShaderPath(const char *aPath)
{
	const std::string strPath(aPath);
	pimpl->mPostprocessor.prependShaderPath(strPath);
}

//---------------------------------------------------------------------
// PostprocessorPimpl Definition
//---------------------------------------------------------------------

struct createEffects
{
    PostprocessorPimpl &mThis;
    createEffects(PostprocessorPimpl *aThis) : mThis(*aThis) {}
    
    template <
        typename tEffect>
    void operator() (
        const tEffect &aEffect) const
    {
        mThis.mEffects.push_back(new Effect(mThis,mThis.mEffects.size(),tEffect::getName()));
    }
};

PostprocessorPimpl::PostprocessorPimpl()
{
    boost::mpl::for_each<t_PostprocessingEffects>(createEffects(this));
}

void PostprocessorPimpl::initEffects()
{
    std::vector<bool> on(mEffects.size(),true);	
    mPostprocessor.init(on);
    BOOST_FOREACH(
        RTfact::Shader::Semantic sem,
        mPostprocessor.getSupportedParameters())
    {
        mParameters.push_back(new PostprocessorParameter(*this,mParameters.size()));
        mParameters.back()->pimpl->mSemantic=sem;
    }
}

RTFACT_REMOTE_NAMESPACE_END

