/* 
 *  Parameterizable.hpp
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
 *  Created on: 2010-09-26 19:11:26 +0200
 *  Author(s): Felix Klein
 */
 
 #ifndef RTFACT_RTPIE__PARAMETRIZABLE_HPP
#define RTFACT_RTPIE__PARAMETRIZABLE_HPP

#include "Config/InternalCommon.hpp"

#include <RTpie/Base/IParameterizable.hpp>

#include <boost/variant.hpp>
#include <string>

RTFACT_RTPIE_FORWARD(Parameterizable)

#include "Base/SceneObject.hpp"

RTFACT_RTPIE_NAMESPACE_BEGIN

/** Datatype which describes all possible parameter values that can be set */
typedef boost::variant<
    int,
    float,
    bool,
    float3,
    std::string,
    IImage2D*,
    unsigned int> ParamValue;

/** Type IDs of every type that can be contained in ParamValue.
*  The order of declaration must be the same because ID should be
*  always equal to ParamValue::which() (i.e. variant<T>::which())
*/
enum ParamValueType
{
    INT_VALUE      = 0,
    FLOAT_VALUE    = 1,
    BOOL_VALUE     = 2,
    FLOAT3_VALUE   = 3,
    STRING_VALUE   = 4,
    IMAGEREF_VALUE = 5,
    UINT_VALUE     = 6
};

#define PARAM_IMPL_SET                      \
    ParamValue v = aValue;                  \
    return setParam(aName, v);              \

#define PARAM_IMPL_SET_T( _type )           \
    ParamValue v = _type(aValue);           \
    return setParam(aName, v);              \

#define PARAM_IMPL_GET(type, type_enum) \
    ParamValue v;                   \
    if(!getParam(aName, v))         \
        return E_INVALIDARG;               \
    if(v.which() != type_enum )      \
        return E_INVALIDARG;               \
    *_retval = boost::get< type >(v);    \
    return S_OK;

class Parameterizable
{
public:

    virtual HRESULT SetParami(const char *aName, const int &aValue){
        PARAM_IMPL_SET
    }

    virtual HRESULT SetParamu(const char *aName, const unsigned int &aValue){
        PARAM_IMPL_SET
    }

    virtual HRESULT SetParamb(const char *aName, const bool &aValue){
        PARAM_IMPL_SET
    }

    virtual HRESULT SetParamf(const char *aName, const float &aValue){
        PARAM_IMPL_SET
    }

    virtual HRESULT SetParam3f(const char *aName, const float &aV1, const float &aV2, const float &aV3)
    {
        float3 value(aV1, aV2, aV3);
        ParamValue v = value;                  
        return setParam(aName, v);              
    }

    virtual HRESULT SetParams(const char *aName, const char *aValue){
        PARAM_IMPL_SET_T(std::string)
    }

    virtual HRESULT SetParamImg(const char *aName, IImage2D *aValue){
        PARAM_IMPL_SET
    }

    virtual HRESULT GetParami(const char *aName, int *_retval){
        PARAM_IMPL_GET(int, INT_VALUE)
    }

    virtual HRESULT GetParamu(const char *aName, unsigned int *_retval){
        PARAM_IMPL_GET(unsigned int, UINT_VALUE)
    }

    virtual HRESULT GetParamb(const char *aName, bool *_retval){
        PARAM_IMPL_GET(bool, BOOL_VALUE)
    }


    virtual HRESULT GetParamf(const char *aName, float *_retval){
        PARAM_IMPL_GET(float, FLOAT_VALUE)
    }

    virtual HRESULT GetParam3f(const char *aName, float *aValue1, float *aValue2, float *aValue3)
	{
        ParamValue v;                   
		if(!getParam(aName, v))         
			return E_INVALIDARG;        
		if(v.which() != FLOAT3_VALUE )     
			return E_INVALIDARG;   
        float3 value = boost::get< float3 >( v );
		*aValue1 = value[0];
        *aValue2 = value[1];
        *aValue3 = value[2];
		return S_OK;
    }

    virtual HRESULT GetParams(const char *aName, char **_retval){
        ParamValue v;
        if(!getParam(aName, v))
            return false;
        if(v.which() != STRING_VALUE )
            return false;
        std::string s = boost::get< std::string >(v);
        *_retval = new char[s.size() + 1];
        strcpy(*_retval, s.c_str());
        return S_OK;
    }


    virtual HRESULT GetParamImg(const char *aName, IImage2D**_retval){
        ParamValue v;
        if(!getParam(aName, v))
            return E_INVALIDARG;
        if(v.which() != IMAGEREF_VALUE )
            return E_INVALIDARG;
        *_retval = boost::get< IImage2D* >(v);
        return S_OK;
    }

    virtual HRESULT HasParam(const char *aName, bool *_retval){
        std::string s(aName);
        *_retval = hasParam(s);
        return S_OK;
    }

    virtual bool setParam(
            const std::string &name,
            const ParamValue &value) = 0;

    virtual bool getParam(
        const std::string &name,
        ParamValue &value) = 0;

    virtual bool hasParam(
        const std::string &name) = 0;

};

#define RTFACT_RTPIE_FORWARD_PARAMETERIZABLE                           \
    virtual HRESULT SetParami(const char *aName, const int &_retval){       \
        return Parameterizable::SetParami(aName, _retval);}              \
    virtual HRESULT SetParamu(const char *aName, const unsigned int &_retval){       \
        return Parameterizable::SetParamu(aName, _retval);}              \
    virtual HRESULT SetParamb(const char *aName, const bool &_retval){      \
        return Parameterizable::SetParamb(aName, _retval);}              \
    virtual HRESULT SetParamf(const char *aName, const float &_retval){     \
        return Parameterizable::SetParamf(aName, _retval);}              \
    virtual HRESULT SetParam3f(const char *aName, const float &aV1, const float &aV2, const float &aV3){   \
        return Parameterizable::SetParam3f(aName, aV1, aV2, aV3);}             \
    virtual HRESULT SetParams(const char *aName, const char *_retval){      \
        return Parameterizable::SetParams(aName, _retval);}              \
    virtual HRESULT SetParamImg(const char *aName, IImage2D *_retval){        \
        return Parameterizable::SetParamImg(aName, _retval);}            \
    virtual HRESULT GetParami(const char *aName, int *_retval){             \
        return Parameterizable::GetParami(aName, _retval);}              \
    virtual HRESULT GetParamu(const char *aName, unsigned int *_retval){             \
        return Parameterizable::GetParamu(aName, _retval);}              \
    virtual HRESULT GetParamb(const char *aName, bool *_retval){            \
        return Parameterizable::GetParamb(aName, _retval);}              \
    virtual HRESULT GetParamf(const char *aName, float *_retval){           \
        return Parameterizable::GetParamf(aName, _retval);}              \
    virtual HRESULT GetParam3f(const char *aName, float *aV1, float *aV2, float *aV3 ){         \
        return Parameterizable::GetParam3f(aName, aV1, aV2, aV3);}             \
    virtual HRESULT GetParams(const char *aName, char **_retval){           \
        return Parameterizable::GetParams(aName, _retval);}              \
    virtual HRESULT GetParamImg(const char *aName, IImage2D** _retval){       \
        return Parameterizable::GetParamImg(aName, _retval);}            \
    virtual HRESULT HasParam(const char *aName, bool *_retval){             \
        return Parameterizable::HasParam(aName, _retval);}



class ParameterizableSceneObject :
    public SceneObject, public Parameterizable
{
public:

    ParameterizableSceneObject(Scene *aScene) : SceneObject(aScene){};
};

RTFACT_RTPIE_NAMESPACE_END

namespace boost
{
    // TODO: There is no macro which define a RTfact namespace name and
    //       RTfact::RTpie namespace name. Currently namespace name is specified
    //       directly.

    //    template <>
    // struct has_nothrow_copy< ::RTfact::Vec3f1>
    //    : mpl::true_
    // {};
    /*
    template <>
    struct has_nothrow_copy< ::RTfact::RTpie::RTImageHandle>
        : mpl::true_
    {};
    */

} // namespace boost

#endif
