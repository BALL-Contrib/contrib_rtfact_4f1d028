/* 
 *  PostprocessorParameter.cpp
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
 *  Author(s): Lukas Marsalek, Felix Klein
 */
 
 #include "Precompiled.hpp"

#include <RTfactRemoteConfig.hpp>
#include <RTfact/Config/Common.hpp>

#define RTFACT_REMOTE_LIB

#include <boost/preprocessor.hpp>

#include <RTremote/PostprocessorParameter.hpp>
#include <RTfact/Utils/Shaders/ParameterConversion.hpp>

#include "RTPrivatePostprocessor.hpp"

RTFACT_POSTPROCESSING_NAMESPACE_BEGIN

PostprocessorParameter::PostprocessorParameter(
    PostprocessorPimpl &aPostprocessor,
    const size_t aIdxParameter)
:
    pimpl(new PostprocessorParameterPimpl(aPostprocessor)),
    mIdxParameter(aIdxParameter)
{}

PostprocessorParameter::~PostprocessorParameter()
{
    delete pimpl;
}

const PostprocessorParameterHandle PostprocessorParameter::getNextParameter() const
{
    if (
        mIdxParameter+1<pimpl->mPostprocessor.mParameters.size())
    {
        return pimpl->mPostprocessor.mParameters[mIdxParameter+1];
    }
    else
    {
        return NULL;
    }
}

const std::string PostprocessorParameter::getName() const
{
    return RTfact::Shader::semantic2str(pimpl->mSemantic);
}

size_t PostprocessorParameter::getValueComponentCount() const
{
    return pimpl->mComponentCount;
}

#define BOOST_PP_ITERATION_PARAMS_1 (3,(0,BOOST_PP_DEC(BOOST_PP_SEQ_SIZE(RTFACT_POSTPROCESSORPARAMETER_TYPES)),"PostprocessorParameterValue.hpp"))
#include BOOST_PP_ITERATE()



PostprocessorParameterPimpl::PostprocessorParameterPimpl(
    PostprocessorPimpl &aPostprocessor)
:
    mPostprocessor(aPostprocessor)
{}

RTFACT_POSTPROCESSING_NAMESPACE_END
