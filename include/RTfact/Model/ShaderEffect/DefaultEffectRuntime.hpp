/* 
 *  DefaultEffectRuntime.hpp
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
 *  Created on: 2010-06-11 12:35:28 +0200
 *  Author(s): Lukas Marsalek, Petr Kmoch
 */
 
 #ifndef RTFACT__DEFAULTEFFECTRUNTIME_HPP
#define RTFACT__DEFAULTEFFECTRUNTIME_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Concept/ShaderEffect.hpp>
#include <RTfact/Utils/Shaders/Parameters.hpp>

RTFACT_NAMESPACE_BEGIN

#define DECLARE_GET_AND_SET_PARAMETER() \
    template < \
        typename tParameterValue> \
    const bool getParameter( \
        const Semantic aParameter, \
        tParameterValue &aValue) const \
    { \
        return false; \
    } \
    \
    template < \
        typename tParameterValue> \
    const bool setParameter( \
        const Semantic aParameter, \
        const tParameterValue &aValue) \
    { \
        return false; \
    } \
    \
    template < \
        typename tParameterValue> \
    const bool getMultiParameter( \
        const Semantic aParameter, \
        tParameterValue *aValue) const \
    { \
        return false; \
    } \
    \
    template < \
        typename tParameterValue> \
    const bool setMultiParameter( \
        const Semantic aParameter, \
        const tParameterValue *aValue) \
    { \
        return false; \
    }

namespace Shader {

/*!
 * Convenience class from which all shader effect runtimes can inherit.
 * Any iheriting class should also declare parameter getting/setting functions
 * by placing the macro DECLARE_GET_AND_SET_PARAMETER() into its public zone.
 * Then, the class can easily specialize parameter getties/setties only for those types which it supports.
 */
class DefaultEffectRuntime :
    public ShaderEffect<Semantic,Parameters>
{
protected:
    /*!
     * A set of Semantics valid for calls to {get|set}[Multi]Parameter.
     * For filling in, the method settableParameters() can be used.
     */
    std::set<Semantic> mSettableParameters;
    
    #define argId() BOOST_PP_CAT(arg,__LINE__)
    
    /*!
     * This method is intended to be called in a constructor of each derived class.
     * All parameters after the first PARAM_IGNORE (inclusive) are ignored.
     * Parameters before that are inserted into mSettableParameters.
     */
    void settableParameters(
        #define SHADER_PARAMETER_MACRO(ma_1,ma_2,ma_3,ma_4) const Semantic argId() = PARAM_IGNORE,
        #include <RTfact/Utils/Shaders/ParameterList.hpp>
        const Semantic argIgnore=PARAM_IGNORE)
    {
        #define SHADER_PARAMETER_MACRO(ma_1,ma_2,ma_3,ma_4) \
            if (argId()!=PARAM_IGNORE) mSettableParameters.insert(argId()); \
            else return;
        #include <RTfact/Utils/Shaders/ParameterList.hpp>
    }
    
    #undef argId

public:
    DECLARE_GET_AND_SET_PARAMETER()
    
    const bool updateParameter(
        const Parameters &aParameters,
        const Semantic aParameter) const
    {
        return false;
    }
    
    void updateParameters(
        const Parameters &aParameters) const
    {}
    
    const std::set<Semantic>& getSettableParameters() const
    {
        return mSettableParameters;
    }
    
    void preRender(
        const Parameters &aParameters)
    {}
    void prePass(
        const size_t aPass,
        const Parameters &aParameters)
    {}
    void postPass(
        const size_t aPass,
        const Parameters &aParameters)
    {}
    void postRender(
        const Parameters &aParameters)
    {}
    
    static const bool mHasPreRender=false;
    static const umask mHasPrePass=0;
    static const umask mHasPostPass=0;
    static const bool mHasPostRender=false;

    static const umask mPrePassClearColor=umask(-1);
    
    static const char ** const getCompileArgs()
    {
        return NULL;
    }
};

} // namespace Shader

RTFACT_NAMESPACE_END

#endif // RTFACT__DEFAULTEFFECTRUNTIME_HPP
