#include "Precompiled.hpp"

#define RTFACT_REMOTE_LIB
#define RTANYSL_LIB
#include <RTfactRemoteConfig.hpp>
#include <RTfact/Config/Common.hpp>
#include <RTremote/Config/Common.hpp>
#include <RTremote/Group.hpp>

#include <RTfact/Model/Image/BasicImage2D.hpp>
#include <RTremote/RTImage.hpp>

#include "RTImageImpl.hpp"
#include "RTAnySLShader.hpp"

#include "RTAnySLShaderBase.hpp"
#include "RTremotePrivateConfig.hpp"

#include <RTAnySL/RTAnySL.hpp>
#include <RTAnySL/ShaderManagerRegistry.hpp>

#include <cassert>
#include <iostream>
#include <boost/cast.hpp>

RTFACT_REMOTE_NAMESPACE_BEGIN

#define RTFACT_REMOTE__ANYSL_PARAM_SHADER_SCRIPT "shaderScript"
#define RTFACT_REMOTE__ANYSL_PARAM_SHADER_LANGUAGE "shaderLanguage"
#define RTFACT_REMOTE__ANYSL_PARAM_SHADER_COMPILER_ERRORCODE \
    "shaderCompilerErrorCode"
#define RTFACT_REMOTE__ANYSL_PARAM_SHADER_COMPILER_OUTPUT \
    "shaderCompilerOutput"

// When ANYSL_RTFACT_BYTE_AND_FLOAT_TEXTURE_SUPPORT is defined type of
// the texture representation is const RTImageImpl*, otherwise only
// float textures are supported and type is const BasicImage2D<float>*.
#define ANYSL_RTFACT_BYTE_AND_FLOAT_TEXTURE_SUPPORT 1

// RTAnySLShader

RTAnySLShader::RTAnySLShader(const size_t aID) :
    RTPrivateAppearance(static_cast<uint>(aID)),
    mShader(new RTAnySLShaderBase<t_Context>()),
    shaderInstance(),
    shaderScript(),
    shaderLanguage(),
    shaderScriptChanged(true),
    specialize(false),
    packetize(false)
{
    RTANYSL_LOG("RTAnySLShader::RTAnySLShader: "
                "Created RTAnySLShader = "<<this);
}

RTAnySLShader::RTAnySLShader(const size_t aID, const std::string & _fileName) :
    RTPrivateAppearance(static_cast<uint>(aID)),
    mShader(new RTAnySLShaderBase<t_Context>()),
    shaderInstance(),
    shaderScript(),
    shaderLanguage(),
    shaderScriptChanged(true),
    specialize(false),
    packetize(false)
{
    RTANYSL_LOG("Created RTAnySLShader = "<<this);

    if (_fileName.length() > 0)
    {
        RTAnySL::ShaderPtr shader = RTAnySL::loadShader(_fileName);

        if (shader)
        {
            shaderLanguage = shader->getManager()->getShadingLanguage();

            RTANYSL_LOG("RTAnySLShader::RTAnySLShader: "
                        "Successfully loaded shader module from "<<_fileName);
            RTANYSL_LOG("RTAnySLShader::RTAnySLShader: "
                        "Shading language : "<<shaderLanguage);

            setShaderInstance(shader->createInstance());
        }
    }
}

RTAnySLShader::~RTAnySLShader()
{
    RTANYSL_LOG("RTAnySLShader::~RTAnySLShader");
}

void RTAnySLShader::setShaderInstance(
    const RTAnySL::ShaderInstancePtr &_shaderInstance)
{
    shaderInstance = _shaderInstance;

    // Setup shader entry table, if there is no shader instance
    // we set entry table to NULL.
    // Note: t_Context type is currently configured RTremote context and
    // is declared in RTremotePrivateConfig.hpp file.

    if (shaderInstance)
    {
        RTAnySLShaderBase<t_Context> * s =
            static_cast< RTAnySLShaderBase<t_Context>* >(&*mShader);

        s->setShaderInstanceContextPtr(&(shaderInstance->getContext()));
        s->setShadeFunc(
            shaderInstance->getGenericShadeFunc<RTfactGenericShadeFunc>());
    }
    else
    {
        RTAnySLShaderBase<t_Context> * s =
            static_cast< RTAnySLShaderBase<t_Context>* >(&*mShader);

        s->setShaderInstanceContextPtr(0);
        s->setShadeFunc(0);
    }
}

void RTAnySLShader::beginParams()
{
}

void RTAnySLShader::endParams()
{
    if (shaderScriptChanged && shaderScript.length() != 0)
    {
        // build new shader instance
        // TODO: currently the same shader manager is used, modify
        //       code to use arbitrary shader manager (over language param)

        RTAnySL::ShaderManagerPtr sm =
            RTAnySL::Singleton<RTAnySL::ShaderManagerRegistry>::getInstance().
                findShaderManager(shaderLanguage);
        if (!sm)
        {
            RTANYSL_ERROR("RTAnySLShader::endParams: "
                          "No ShaderManager found for \""
                          <<shaderLanguage<<"\" shading language");
            return;
        }
        //getShaderInstance()->getShader()->getManager();
        RTAnySL::ShaderPtr newShader = sm->buildShader(
                shaderScript, &shaderCompilerResult);

        if (newShader)
        {
            RTAnySL::ShaderInstancePtr newInstance =
                newShader->createInstance();

            RTAnySL::OptionsList options;

            if (getShaderInstance())
            {
                // if we have already shader instance
                // copy its options to the new one

                getShaderInstance()->getOptions(options);
            }
            else
            {
                options.push_back(RTAnySL::Option(RTANYSL_PACKETIZE_OPTION_NAME, packetize));
                options.push_back(RTAnySL::Option(RTANYSL_SPECIALIZE_OPTION_NAME, specialize));
            }

            // copying of the parameters might cause incorrect results
            // as it overwrites the defaults of the new shader instance,
            // so instead we just set cached parameters.

            newInstance->beginParams();
            if (!options.empty())
                newInstance->setOptions(options);
            processCachedParams(newInstance, false);
            newInstance->endParams();

            setShaderInstance(newInstance);
        }

        shaderScriptChanged = false;
    }
    else
    {
        // only normal parameters changed, so set all options
        // and process cache
        if (getShaderInstance())
        {
            getShaderInstance()->beginParams();

            getShaderInstance()->setOption(
                    RTANYSL_PACKETIZE_OPTION(packetize));
            getShaderInstance()->setOption(
                    RTANYSL_SPECIALIZE_OPTION(specialize));

            processCachedParams(getShaderInstance(), false);
            getShaderInstance()->endParams();
        }
    }
}

bool RTAnySLShader::setParam(
    const std::string &name,
    const ParamValue &value)
{
    //enable / disable packetization
    if (name == "vectorize" || name == "packetize")
    {
        const bool *bval = boost::get<bool>(&value);
        if (bval)
        {
            packetize = *bval;
            if (getShaderInstance())
            {
                getShaderInstance()->setOption(
                        RTANYSL_PACKETIZE_OPTION(packetize));
            }
        }
        return true;
    }
    else if (name == "specialize")
    {
        const bool *bval = boost::get<bool>(&value);
        if (bval)
        {
            specialize = *bval;
            if (getShaderInstance())
            {
                getShaderInstance()->setOption(
                        RTANYSL_SPECIALIZE_OPTION(specialize));
            }
        }
        return true;
    }
    else if (name == RTFACT_REMOTE__ANYSL_PARAM_SHADER_SCRIPT)
    {
        // we will process script in endParams,
        // when all parameters are processed
        const std::string *sval = boost::get<std::string>(&value);
        if (sval)
        {
            if (*sval != shaderScript)
            {
                shaderScriptChanged = true;
                shaderScript = *sval;
            }
            return true;
        }
        return false;
    }
    else if (name == RTFACT_REMOTE__ANYSL_PARAM_SHADER_LANGUAGE)
    {
        if (const std::string *sval = boost::get<std::string>(&value))
        {
            shaderLanguage = *sval;
            return true;
        }
        return false;
    }
    else if (name == RTFACT_REMOTE__ANYSL_PARAM_SHADER_COMPILER_ERRORCODE ||
             name == RTFACT_REMOTE__ANYSL_PARAM_SHADER_COMPILER_OUTPUT)
    {
        // compiler error code and output are read-only parameters
        return false;
    }
    else
    {
        // save shader parameter for deferred processing
        paramDataCache.push_back(ParamData(name, value));
        return true;
    }
    return false;
}

bool RTAnySLShader::getParam(
    const std::string &name,
    ParamValue &value)
{
    // enable / disable packetization
    if (name == "vectorize" || name == "packetize")
    {
        if (getShaderInstance())
        {
            // TODO
        }
        return false;
    }
    else if (name == "specialize")
    {
        if (getShaderInstance())
        {
            // TODO
        }
        return false;
    }
    else if (name == RTFACT_REMOTE__ANYSL_PARAM_SHADER_SCRIPT)
    {
        value = shaderScript;
        return true;
    }
    else if (name == RTFACT_REMOTE__ANYSL_PARAM_SHADER_LANGUAGE)
    {
        value = shaderLanguage;
        return true;
    }
    else if (name == RTFACT_REMOTE__ANYSL_PARAM_SHADER_COMPILER_ERRORCODE)
    {
        value = shaderCompilerResult.getErrorCode();
        return true;
    }
    else if (name == RTFACT_REMOTE__ANYSL_PARAM_SHADER_COMPILER_OUTPUT)
    {
        value = shaderCompilerResult.getOutput();
        return true;
    }
    else
    {
        if (getShaderInstance())
        {
            int paramIndex = getShaderInstance()->getParamIndex(name);
            if (paramIndex != -1)
            {
                RTAnySL::ParameterInfo pinfo =
                    getShaderInstance()->getParamInfo(paramIndex);

                if (pinfo.isOfType<char>())
                {
                    char pvalue = getShaderParam<char>(paramIndex);
                    if (pinfo.semantic == RTAnySL::ParameterInfo::BOOLEAN_SEMANTIC)
                        value = (pvalue != 0);
                    else
                        value = (pvalue != 0);
                }
                else if (pinfo.isOfType<int>())
                {
                    int pvalue = getShaderParam<int>(paramIndex);
                    if (pinfo.semantic == RTAnySL::ParameterInfo::BOOLEAN_SEMANTIC)
                        value = (pvalue != 0);
                    else
                        value = (pvalue != 0);
                    return true;
                }
                else if (pinfo.isOfType<float>())
                {
                    value = getShaderParam<float>(paramIndex);
                    return true;
                }
                else if (pinfo.getNumArrayElements() == 3 &&
                         pinfo.isElementOfType<float>())
                {
                    value = getShaderParam<float3>(paramIndex);
                    return true;
                }
                else if (pinfo.isOfType<char*>())
                {
                    RTANYSL_WARNING("Getting of string value"
                                    " from shader parameter \""<<name
                                    <<"\" is not supported yet");
                    return false;
                }
                // TODO: RTImageHandle
                // TODO: GroupHandle
            }
            else
            {
                RTANYSL_WARNING("(!!!) Tried to get unknown parameter "<<name);
            }
        }
    }
    return false;
}

void RTAnySLShader::getParamInfos(
    std::vector<ParamInfo> &infos)
{
    if (getShaderInstance())
    {
        RTAnySL::ParameterInfoVector anyslInfos;

        getShaderInstance()->getParamInfos(anyslInfos);

        typedef RTAnySL::ParameterInfoVector::iterator Iter;
        for (Iter it = anyslInfos.begin(); it != anyslInfos.end(); ++it)
        {

            ParamValueType t = VOIDPTR_VALUE;

            if (it->isOfType<int>())
            {
                if (it->semantic == RTAnySL::ParameterInfo::BOOLEAN_SEMANTIC)
                    t = BOOL_VALUE;
                else
                    t = INT_VALUE;
            }
            else if (it->isOfType<float>() || it->isOfType<double>())
                t = FLOAT_VALUE;
            else if (it->getNumArrayElements() == 3 &&
                     (it->isElementOfType<float>() ||
                      it->isElementOfType<double>()))
                t = FLOAT3_VALUE;
            else if (it->isOfType<char*>())
                t = STRING_VALUE;
            else if (it->semantic == RTAnySL::ParameterInfo::TEXTURE_SEMANTIC &&
                     it->type.isPointerType)
            {
                t = IMAGEREF_VALUE;
            }

            infos.push_back(ParamInfo(it->name, t, it->description));
        }
    }
    infos.push_back(ParamInfo(
            RTFACT_REMOTE__ANYSL_PARAM_SHADER_SCRIPT, STRING_VALUE,
            "shader script"));
    infos.push_back(ParamInfo(
            RTFACT_REMOTE__ANYSL_PARAM_SHADER_LANGUAGE, STRING_VALUE,
            "shading language of the shader script"));
    infos.push_back(ParamInfo(
            RTFACT_REMOTE__ANYSL_PARAM_SHADER_COMPILER_ERRORCODE, INT_VALUE,
            "shader compiler error code"));
    infos.push_back(ParamInfo(
            RTFACT_REMOTE__ANYSL_PARAM_SHADER_COMPILER_OUTPUT, STRING_VALUE,
            "shader compiler error output"));
    infos.push_back(ParamInfo("specialize", BOOL_VALUE, "specialize shader"));
    infos.push_back(ParamInfo("packetize", BOOL_VALUE, "packetize shader"));
}

bool RTAnySLShader::hasParam(
    const std::string &name)
{
    return getShaderInstance() ? getShaderInstance()->hasParam(name) : false;
}

void RTAnySLShader::processCachedParams(
        const RTAnySL::ShaderInstancePtr &shaderInstance,
        bool doBeginAndEndParams)
{
    if (shaderInstance)
    {
        if (doBeginAndEndParams)
            shaderInstance->beginParams();

        for (ParamDataCache::iterator it = paramDataCache.begin(); it != paramDataCache.end(); ++it)
        {
            setShaderParam(shaderInstance, it->name, it->value);
        }

        if (doBeginAndEndParams)
            shaderInstance->endParams();

        paramDataCache.clear();
    }
}

bool RTAnySLShader::setShaderParam(
        const RTAnySL::ShaderInstancePtr &shaderInstance,
        const std::string &name,
        ParamValue &value)
{
    if (const int *ival = boost::get<int>(&value))
    {
        return setRawShaderParam(
            shaderInstance, name,
            static_cast<const void*>(ival), sizeof(int));
    }
    if (const float *fval = boost::get<float>(&value))
    {
        return setRawShaderParam(
            shaderInstance, name,
            static_cast<const void*>(fval), sizeof(float));
    }
    if (const float3 *f3val = boost::get<float3>(&value))
    {
        Vec3f1 tmp(f3val->x, f3val->y, f3val->z);

        return setRawShaderParam(
            shaderInstance, name,
            static_cast<const void*>(&tmp), 3 * sizeof(float));
    }
    if (const bool *bval = boost::get<bool>(&value))
    {
        int ival = (int) *bval;
        return setRawShaderParam(
            shaderInstance, name,
            static_cast<const void*>(&ival), sizeof(int));
    }
    if (const std::string *sval = boost::get<std::string>(&value))
    {
        // TODO: std::string
        // currently we just ignore it
        RTANYSL_WARNING("RTAnySLShader::setShaderParam:"
                        " Setting of string value \""<<(*sval)
                        <<"\" to shader parameter \""<<name
                        <<"\" is not supported yet");
        return false;
    }
    if (const RTImageHandle* tex = boost::get<RTImageHandle>(&value))
    {
        RTImageHandle image = *const_cast<RTImageHandle*>(tex);

#ifndef ANYSL_RTFACT_BYTE_AND_FLOAT_TEXTURE_SUPPORT
        if(image->getComponentType() != RTImage::COMPONENT_FLOAT)
        {
            RTANYSL_WARNING("RTAnySLShader:: RTfact/AnySL currently only supports float textures");
            return false;
        }

        Traits<BasicImage2D<float> >::Ptr basicimage =
            boost::static_pointer_cast<BasicImage2D<float> >(
                    boost::polymorphic_cast<RTImageImpl*>(image.get())->getRTfactImage());

        // save image in a texture cache, so it can't be destroyed when rendered
        imageCache[name] = image;

        return setRawShaderParam(
            shaderInstance, name,
            &paramm, sizeof(BasicImage2D<float>*));
#else
        // save image in a texture cache, so it can't be destroyed when rendered
        imageCache[name] = image;

        RTImageImpl *basicimage =
                    boost::polymorphic_cast<RTImageImpl*>(image.get());

        const void* paramm = static_cast<const void*>(basicimage);

        const Traits<Image2D>::Ptr &rtfactImage = basicimage->getRTfactImage();

        return setRawShaderParam(
            shaderInstance, name,
            &paramm, sizeof(RTImageImpl*));
#endif

    }
    // TODO: GroupHandle
    return false;
}

bool RTAnySLShader::setRawShaderParam(
    const RTAnySL::ShaderInstancePtr &shaderInstance,
    const std::string &name,
    const void *data,
    int byteSize)
{
    RTANYSL_ASSERT(shaderInstance != 0, "shader instance is NULL");

    int index = shaderInstance->getParamIndex(name);
    if (index != -1)
    {
        bool ok = shaderInstance->setParam(index, data, byteSize);
        if (!ok)
        {
            RTANYSL_WARNING("RTAnySLShader::setRawShaderParam: (!!!) Could not set parameter "<<name<<" size="<<byteSize<<". Possibly size mismatch.");
        }
        return ok;
    }
    else
    {
        RTANYSL_WARNING("RTAnySLShader::setRawShaderParam: (!!!) Tried to set unknown parameter "<<name);
    }

    return false;
}

RTFACT_REMOTE_NAMESPACE_END
