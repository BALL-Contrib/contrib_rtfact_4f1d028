#include "Precompiled.hpp"

#define RTANYSL_LIB
#include <RTAnySL/Config/Common.hpp>

#include <RTAnySL/Shader.hpp>
#include <RTAnySL/Exception.hpp>
#include <sstream>

namespace RTAnySL
{

Shader::Shader() :
    module(0),
    parentShader(),
    shaderManager()
{
}

Shader::Shader(const ShaderManagerPtr &_shaderManager,
               const ShaderPtr &_parentShader,
               llvm::Module * _module) :
    shaderManager(_shaderManager),
    parentShader(_parentShader),
    module(_module)
{
}

Shader::~Shader()
{
}

void Shader::setModule(llvm::Module * _module)
{
    module.reset(_module);
}

llvm::Module * Shader::releaseModule()
{
    return module.release();
}

bool Shader::hasParam(const std::string &name) const
{
    return (module.get() != 0 ?
            jitRT::getGlobalVariable(module.get(), name) != NULL : false);
}

} // namespace RTAnySL
