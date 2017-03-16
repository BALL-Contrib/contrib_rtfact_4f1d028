#include "Precompiled.hpp"

#define RTANYSL_LIB
#include <RTAnySL/Config/Common.hpp>

#include <RTAnySL/ShaderManagerRegistry.hpp>
#include <RTAnySL/ShaderManager.hpp>
#include <RTAnySL/Exception.hpp>
#include <RTAnySL/Utils.hpp>
#include <sstream>

namespace RTAnySL
{

RTANYSL_DEFINE_SINGLETON(ShaderManagerRegistry)

ShaderManagerRegistry::ShaderManagerRegistry()
{
    RTANYSL_DEBUG("ShaderManagerRegistry created");
}

ShaderManagerRegistry::~ShaderManagerRegistry()
{
    RTANYSL_DEBUG("ShaderManagerRegistry destroyed");
}

void ShaderManagerRegistry::registerShaderManager(
    const std::string &shadingLanguage,
    const ShaderManagerPtr &shaderManager)
{
    if (shaderManager == 0)
        throw Exception("Cannot register NULL shader manager");

    if (shaderManagerMap.find(shadingLanguage) != shaderManagerMap.end())
    {
        std::ostringstream oss;
        oss<<"Shader manager for language \""<<shadingLanguage
           <<"\" is already registered";
        throw Exception(oss.str());
    }
    shaderManagerMap[shadingLanguage] = shaderManager;
}


void ShaderManagerRegistry::unregisterShaderManager(
    const ShaderManagerPtr &shaderManager)
{
    ShaderManagerMap::iterator it = shaderManagerMap.begin();
    while (it != shaderManagerMap.end())
    {
        if (it->second == shaderManager)
        {
            ShaderManagerMap::iterator next = it;
            ++next;

            shaderManagerMap.erase(it);
            it = next;
        }
        else
            ++it;
    }
}

void ShaderManagerRegistry::unregisterShaderManager(
    const std::string &shadingLanguage)
{
    shaderManagerMap.erase(shadingLanguage);
}

ShaderManagerPtr ShaderManagerRegistry::findShaderManager(
    const std::string &shadingLanguage) const
{
    ShaderManagerMap::const_iterator it =
        shaderManagerMap.find(shadingLanguage);
    return it != shaderManagerMap.end() ? it->second : ShaderManagerPtr();
}

ShaderManagerPtr ShaderManagerRegistry::findShaderManager(
    llvm::Module *module) const
{
    RTANYSL_ASSERT(module != 0, "Module cannot be NULL");

    for (ShaderManagerMap::const_iterator it = shaderManagerMap.begin();
         it != shaderManagerMap.end(); ++it)
    {
        if (it->second->isModuleSupported(module))
            return it->second;
    }
    return ShaderManagerPtr();
}

void ShaderManagerRegistry::clear()
{
    shaderManagerMap.clear();
}

} // namespace RTAnySL
