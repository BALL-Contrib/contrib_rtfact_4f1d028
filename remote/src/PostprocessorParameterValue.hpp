/* 
 *  PostprocessorParameterValue.hpp
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
 
 #if BOOST_PP_ITERATION_DEPTH() == 1 //type iteration

#define ARG_TYPE() BOOST_PP_SEQ_ELEM(BOOST_PP_FRAME_ITERATION(1),RTFACT_POSTPROCESSORPARAMETER_TYPES)

#define BOOST_PP_ITERATION_PARAMS_2 (3,(1,4,"PostprocessorParameterValue.hpp"))
#include BOOST_PP_ITERATE()

void PostprocessorParameter::getValue(ARG_TYPE() *aValue) const
{
    switch (
        pimpl->mSemantic)
    {
        #define IGNORE_ARGS4(ma1,ma2,ma3,ma4)
        #define SAME_MULTI(maSemantic,maName,maType,maComponents) \
            pimpl->mPostprocessor.mPostprocessor.getMultiParameter(pimpl->mSemantic,aValue);
        #define SAME_SINGLE(maSemantic,maName,maType,maComponents) \
            pimpl->mPostprocessor.mPostprocessor.getParameter(pimpl->mSemantic,*aValue);
        #define DIFFERENT_MULTI(maSemantic,maName,maType,maComponents) \
            maType val[maComponents]; \
            pimpl->mPostprocessor.mPostprocessor.getMultiParameter(pimpl->mSemantic,val); \
            for ( \
                size_t idx=0; idx<maComponents; ++idx) \
                RTfact::Shader::ParameterConverter<maType,ARG_TYPE() >::convert(val[idx],*aValue++);
        #define DIFFERENT_SINGLE(maSemantic,maName,maType,maComponents) \
            maType val; \
            pimpl->mPostprocessor.mPostprocessor.getParameter(pimpl->mSemantic,val); \
            RTfact::Shader::ParameterConverter<maType,ARG_TYPE() >::convert(val,*aValue);
        #define SEMANTIC_CASE(maSemantic,maName,maType,maComponents) \
            case RTfact::Shader::PARAM_##maSemantic: \
                if ( \
                    boost::is_same<ARG_TYPE(),maType >::value) \
                { \
                    BOOST_PP_IF(BOOST_PP_GREATER(maComponents,1),SAME_MULTI,SAME_SINGLE)(maSemantic,maName,maType,maComponents) \
                } \
                else \
                { \
                    BOOST_PP_IF(BOOST_PP_GREATER(maComponents,1),DIFFERENT_MULTI,DIFFERENT_SINGLE)(maSemantic,maName,maType,maComponents) \
                } \
                break;

        #define SHADER_PARAMETER_MACRO(maSemantic,maName,maType,maComponents) \
            BOOST_PP_IF(maComponents,SEMANTIC_CASE,IGNORE_ARGS4)(maSemantic,maName,maType,maComponents)
        #include <RTfact/Utils/Shaders/ParameterList.hpp>
        
        #undef SEMANTIC_CASE
        #undef IGNORE_ARGS4
        #undef SAME_MULTI
        #undef SAME_SINGLE
        #undef DIFFERENT_MULTI
        #undef DIFFERENT_SINGLE
    }
}

const bool PostprocessorParameter::setValue(const ARG_TYPE() *aValue)
{
    switch (
        pimpl->mSemantic)
    {
        #define IGNORE_ARGS4(ma1,ma2,ma3,ma4)
        #define SAME_MULTI(maSemantic,maName,maType,maComponents) \
            return pimpl->mPostprocessor.mPostprocessor.setMultiParameter(pimpl->mSemantic,aValue);
        #define SAME_SINGLE(maSemantic,maName,maType,maComponents) \
            return pimpl->mPostprocessor.mPostprocessor.setParameter(pimpl->mSemantic,*aValue);
        #define DIFFERENT_MULTI(maSemantic,maName,maType,maComponents) \
            maType val[maComponents]; \
            for ( \
                size_t idx=0; idx<maComponents; ++idx) \
                RTfact::Shader::ParameterConverter<ARG_TYPE(),maType >::convert(*aValue++,val[idx]); \
            return pimpl->mPostprocessor.mPostprocessor.setMultiParameter(pimpl->mSemantic,val);
        #define DIFFERENT_SINGLE(maSemantic,maName,maType,maComponents) \
            maType val; \
            RTfact::Shader::ParameterConverter<ARG_TYPE(),maType >::convert(*aValue,val); \
            return pimpl->mPostprocessor.mPostprocessor.setParameter(pimpl->mSemantic,val);
        #define SEMANTIC_CASE(maSemantic,maName,maType,maComponents) \
            case RTfact::Shader::PARAM_##maSemantic: \
                if ( \
                    boost::is_same<ARG_TYPE(),maType >::value) \
                { \
                    BOOST_PP_IF(BOOST_PP_GREATER(maComponents,1),SAME_MULTI,SAME_SINGLE)(maSemantic,maName,maType,maComponents) \
                } \
                else \
                { \
                    BOOST_PP_IF(BOOST_PP_GREATER(maComponents,1),DIFFERENT_MULTI,DIFFERENT_SINGLE)(maSemantic,maName,maType,maComponents) \
                } \
                break;

        #define SHADER_PARAMETER_MACRO(maSemantic,maName,maType,maComponents) \
            BOOST_PP_IF(maComponents,SEMANTIC_CASE,IGNORE_ARGS4)(maSemantic,maName,maType,maComponents)
        #include <RTfact/Utils/Shaders/ParameterList.hpp>
        
        #undef SEMANTIC_CASE
        #undef IGNORE_ARGS4
        #undef SAME_MULTI
        #undef SAME_SINGLE
        #undef DIFFERENT_MULTI
        #undef DIFFERENT_SINGLE
        
        default:
            assert(false);  //Should never happen
            return false;
    }
}

#undef ARG_TYPE

#elif BOOST_PP_ITERATION_DEPTH() == 2 //argument count iteration

#define ARG_COUNT() BOOST_PP_ITERATION()

#define SET_ARG(maZ,maIdx,maType) \
    RTfact::Shader::ParameterConverter<maType,ARG_TYPE() >::convert(val[maIdx],aValue##maIdx);

void PostprocessorParameter::getValue(BOOST_PP_ENUM_PARAMS(ARG_COUNT(),ARG_TYPE() &aValue)) const
{
    switch (
        pimpl->mSemantic)
    {
        #define IGNORE_ARGS4(ma1,ma2,ma3,ma4)
        #define SAME_MULTI(maSemantic,maName,maType,maComponents) \
            maType val[maComponents]; \
            pimpl->mPostprocessor.mPostprocessor.getMultiParameter(pimpl->mSemantic,val); \
            BOOST_PP_REPEAT(BOOST_PP_MIN(ARG_COUNT(),maComponents),SET_ARG,maType)
        #define SAME_SINGLE(maSemantic,maName,maType,maComponents) \
            pimpl->mPostprocessor.mPostprocessor.getParameter(pimpl->mSemantic,aValue0);
        #define DIFFERENT_MULTI(maSemantic,maName,maType,maComponents) \
            maType val[maComponents]; \
            pimpl->mPostprocessor.mPostprocessor.getMultiParameter(pimpl->mSemantic,val); \
            BOOST_PP_REPEAT(BOOST_PP_MIN(maComponents,ARG_COUNT()),SET_ARG,maType)
        #define DIFFERENT_SINGLE(maSemantic,maName,maType,maComponents) \
            maType val; \
            pimpl->mPostprocessor.mPostprocessor.getParameter(pimpl->mSemantic,val); \
            RTfact::Shader::ParameterConverter<maType,ARG_TYPE() >::convert(val,aValue0);
        #define SEMANTIC_CASE(maSemantic,maName,maType,maComponents) \
            case RTfact::Shader::PARAM_##maSemantic: \
                if ( \
                    boost::is_same<ARG_TYPE(),maType >::value) \
                { \
                    BOOST_PP_IF(BOOST_PP_GREATER(maComponents,1),SAME_MULTI,SAME_SINGLE)(maSemantic,maName,maType,maComponents) \
                } \
                else \
                { \
                    BOOST_PP_IF(BOOST_PP_GREATER(maComponents,1),DIFFERENT_MULTI,DIFFERENT_SINGLE)(maSemantic,maName,maType,maComponents) \
                } \
                break;

        #define SHADER_PARAMETER_MACRO(maSemantic,maName,maType,maComponents) \
            BOOST_PP_IF(maComponents,SEMANTIC_CASE,IGNORE_ARGS4)(maSemantic,maName,maType,maComponents)
        #include <RTfact/Utils/Shaders/ParameterList.hpp>
        
        #undef SEMANTIC_CASE
        #undef IGNORE_ARGS4
        #undef SAME_MULTI
        #undef SAME_SINGLE
        #undef DIFFERENT_MULTI
        #undef DIFFERENT_SINGLE
    }
}

#undef SET_ARG

#define SET_ARG(maZ,maIdx,maType) \
    RTfact::Shader::ParameterConverter<ARG_TYPE(),maType >::convert(aValue##maIdx,val[maIdx]);

const bool PostprocessorParameter::setValue(BOOST_PP_ENUM_PARAMS(ARG_COUNT(),const ARG_TYPE() aValue))
{
    switch (
        pimpl->mSemantic)
    {
        #define IGNORE_ARGS4(ma1,ma2,ma3,ma4)
        #define SAME_MULTI(maSemantic,maName,maType,maComponents) \
            maType val[maComponents]; \
            BOOST_PP_REPEAT(maComponents,SET_ARG,maType) \
            return pimpl->mPostprocessor.mPostprocessor.setMultiParameter(pimpl->mSemantic,val);
        #define SAME_SINGLE(maSemantic,maName,maType,maComponents) \
            return pimpl->mPostprocessor.mPostprocessor.setParameter(pimpl->mSemantic,aValue0);
        #define DIFFERENT_MULTI(maSemantic,maName,maType,maComponents) \
            maType val[maComponents]; \
            BOOST_PP_REPEAT(maComponents,SET_ARG,maType) \
            return pimpl->mPostprocessor.mPostprocessor.setMultiParameter(pimpl->mSemantic,val);
        #define DIFFERENT_SINGLE(maSemantic,maName,maType,maComponents) \
            maType val; \
            RTfact::Shader::ParameterConverter<ARG_TYPE(),maType >::convert(aValue0,val); \
            return pimpl->mPostprocessor.mPostprocessor.setParameter(pimpl->mSemantic,val);
        #define SEMANTIC_CASE(maSemantic,maName,maType,maComponents) \
            case RTfact::Shader::PARAM_##maSemantic: \
                if ( \
                    ARG_COUNT()<maComponents) \
                { \
                    return false; \
                } \
                if ( \
                    boost::is_same<ARG_TYPE(),maType >::value) \
                { \
                    BOOST_PP_IF(BOOST_PP_GREATER(maComponents,1),SAME_MULTI,SAME_SINGLE)(maSemantic,maName,maType,maComponents) \
                } \
                else \
                { \
                    BOOST_PP_IF(BOOST_PP_GREATER(maComponents,1),DIFFERENT_MULTI,DIFFERENT_SINGLE)(maSemantic,maName,maType,maComponents) \
                } \
                break;

        #define SHADER_PARAMETER_MACRO(maSemantic,maName,maType,maComponents) \
            BOOST_PP_IF(maComponents,SEMANTIC_CASE,IGNORE_ARGS4)(maSemantic,maName,maType,maComponents)
        #include <RTfact/Utils/Shaders/ParameterList.hpp>
        
        #undef SEMANTIC_CASE
        #undef IGNORE_ARGS4
        #undef SAME_MULTI
        #undef SAME_SINGLE
        #undef DIFFERENT_MULTI
        #undef DIFFERENT_SINGLE
        
        default:
            assert(false);  //Should never happen
            return false;
    }
}

#undef SET_ARG

#undef ARG_COUNT

#endif
