#include "Precompiled.hpp"

#define RTANYSL_LIB
#include <RTAnySL/Config/Common.hpp>

#include <RTAnySL/ShaderInstance.hpp>
#include <RTAnySL/Shader.hpp>
#include <RTAnySL/Exception.hpp>
#include <sstream>

namespace RTAnySL
{

ShaderInstance::ShaderInstance() :
    shader()
{
}

ShaderInstance::ShaderInstance(const ShaderPtr &shader) :
    shader(shader)
{
}

ShaderInstance::~ShaderInstance()
{
}

} // namespace RTAnySL
