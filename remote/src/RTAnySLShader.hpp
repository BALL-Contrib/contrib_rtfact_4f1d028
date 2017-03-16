#ifndef RTFACT_REMOTE__ANYSLSHADER_HPP
#define RTFACT_REMOTE__ANYSLSHADER_HPP

#include <RTremote/Config/Common.hpp>

#include <boost/pointer_cast.hpp>

#include "RTPrivateAppearance.hpp"
#include <RTAnySL/RTAnySL.hpp>
#include <RTAnySL/Shader.hpp>
#include <RTAnySL/ShaderManager.hpp>
#include <RTAnySL/ShaderInstance.hpp>
#include <RTAnySL/Utils.hpp>

RTFACT_REMOTE_NAMESPACE_BEGIN

class RTAnySLShader : public RTPrivateAppearance
{
    friend class Renderer;
public:

    virtual ~RTAnySLShader();

    const RTAnySL::ShaderInstancePtr & getShaderInstance() const
    {
        return shaderInstance;
    }

    void setShaderInstance(const RTAnySL::ShaderInstancePtr &shaderInstance);

    virtual void beginParams();

    virtual void endParams();

    virtual bool setParam(
        const std::string &name,
        const ParamValue &value);

    virtual bool getParam(
        const std::string &name,
        ParamValue &value);

    virtual void getParamInfos(
        std::vector<ParamInfo> &infos);

    virtual bool hasParam(
        const std::string &name);

protected:

    struct ParamData
    {
        std::string name;
        ParamValue value;

        ParamData() { }

        ParamData(const std::string &name, const ParamValue &value) :
            name(name), value(value)
        {
        }
    };
    typedef std::vector<ParamData> ParamDataCache;

    Traits<RTfact::Appearance>::Ptr mShader;
    RTAnySL::ShaderInstancePtr shaderInstance;
    RTAnySL::ShaderCompilerResult shaderCompilerResult;

    // RTAnySL-specific shader parameters
    std::string shaderScript;
    std::string shaderLanguage;
    bool shaderScriptChanged;

    // options
    bool specialize;
    bool packetize;

    // temporary storage for parameters
    ParamDataCache paramDataCache;
    std::map<std::string, RTImageHandle> imageCache;

    virtual Traits<RTfact::Appearance>::Ptr getRTfactAppearance()
    {
        return mShader;
    }

    RTAnySLShader(const size_t aID);

    RTAnySLShader(const size_t aID, const std::string & _fileName);

    void processCachedParams(
        const RTAnySL::ShaderInstancePtr &shaderInstance,
        bool doBeginAndEndParams);

    template <class T> T getShaderParam(int index)
    {
        T destValue;
        getShaderInstance()->getParam(index, &destValue, sizeof(T));
        return destValue;
    }

    bool setShaderParam(
        const RTAnySL::ShaderInstancePtr &shaderInstance,
        const std::string &name, ParamValue &value);

    static bool setRawShaderParam(
        const RTAnySL::ShaderInstancePtr &shaderInstance,
        const std::string &name,
        const void *data,
        int byteSize);
};

RTFACT_REMOTE_NAMESPACE_END

#endif
