#ifndef RTFACT_REMOTE__PHONGSHADERSL_HPP
#define RTFACT_REMOTE__PHONGSHADERSL_HPP

#include <RTremote/Config/Common.hpp>

#include <boost/pointer_cast.hpp>

#include "RTPrivateAppearance.hpp"
#include "RTImageImpl.hpp"

#include <RTfact/Model/SurfaceShader/TestShader.hpp>

RTFACT_REMOTE_NAMESPACE_BEGIN

template<
    class tContext>
class RTTestShader :
    public RTPrivateAppearance
{
    friend class Renderer;

protected:

    typedef TestShader<tContext> t_Shader;

    typename Traits<t_Shader>::Ptr mShader;

    virtual Traits<RTfact::Appearance>::Ptr getRTfactAppearance()
    {
        return mShader;
    }

    RTTestShader(
        const size_t aID
    ) :
        RTPrivateAppearance(static_cast<uint>(aID)),
        mShader(new t_Shader)
    {}

    RTTestShader(
        const RTTestShader&);

public:

    virtual bool hasNewAppearance()
    {
        return false;
    }

    virtual void setAppearanceUpdated()
    {}

    virtual bool setParam(
        const std::string &name,
        const ParamValue &value)
    {
        return false;
    }

    virtual bool getParam(
        const std::string &name,
        ParamValue &value)
    {
        return false;
    }

    virtual bool hasParam(
        const std::string &name)
    {
        return false;
    }

    virtual void getParamInfos(
        std::vector<ParamInfo> &infos)
    {
        return;
    }
};

RTFACT_REMOTE_NAMESPACE_END

#endif // RTFACT_REMOTE__PHONGSHADERSL_HPP
