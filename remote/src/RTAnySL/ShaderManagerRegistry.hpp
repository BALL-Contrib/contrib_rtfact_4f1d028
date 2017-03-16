#ifndef RTANYSL_SHADERMANAGERREGISTRY_HPP_INCLUDED
#define RTANYSL_SHADERMANAGERREGISTRY_HPP_INCLUDED

#include <RTAnySL/Config/Common.hpp>
#include <map>
#include <RTAnySL/ShaderManagerFwd.hpp>
#include <RTAnySL/Singleton.hpp>
#include <jitRT/llvmWrapper.h>

namespace RTAnySL
{

/// This is base class for every shader manager
class RTANYSL_API ShaderManagerRegistry
{
    RTANYSL_DECLARE_DEFAULT_SINGLETON(ShaderManagerRegistry)
public:

    void registerShaderManager(const std::string &shadingLanguage,
                               const ShaderManagerPtr &shaderManager);

    void unregisterShaderManager(const std::string &shadingLanguage);

    void unregisterShaderManager(const ShaderManagerPtr &shaderManager);

    ShaderManagerPtr findShaderManager(const std::string &shadingLanguage) const;

    ShaderManagerPtr findShaderManager(llvm::Module *module) const;

    void clear();

private:

    /// maps shading language to shader manager
    typedef std::map<std::string, ShaderManagerPtr> ShaderManagerMap;

    ShaderManagerMap shaderManagerMap;

    ShaderManagerRegistry();
    ~ShaderManagerRegistry();

};

#define RTANYSL_REGISTER_SHADER_MANAGER(ShaderManagerType)              \
{                                                                       \
    RTAnySL::ShaderManagerPtr _sm = RTAnySL::ShaderManagerPtr(          \
        new ShaderManagerType());                                       \
    RTAnySL::Singleton<RTAnySL::ShaderManagerRegistry>::getInstance().  \
        registerShaderManager(_sm->getShadingLanguage(), _sm);          \
}

/// args need to be enclosed in braces '(' ')'
#define RTANYSL_REGISTER_SHADER_MANAGER_WITH_ARGS(ShaderManagerType, args) \
{                                                                          \
    RTAnySL::ShaderManagerPtr _sm = RTAnySL::ShaderManagerPtr(             \
        new ShaderManagerType args);                                       \
    RTAnySL::Singleton<RTAnySL::ShaderManagerRegistry>::getInstance().     \
        registerShaderManager(_sm->getShadingLanguage(), _sm);             \
}

} // namespace RTAnySL

#endif
