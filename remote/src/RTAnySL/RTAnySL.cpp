#include "Precompiled.hpp"

#define RTANYSL_LIB
#include <RTAnySL/Config/Common.hpp>

#include <RTAnySL/RTAnySL.hpp>
#include <RTAnySL/Exception.hpp>
#include <RTAnySL/Utils.hpp>
#include <RTAnySL/ModuleAutoPtr.hpp>
#include <RTAnySL/ShaderManager.hpp>
#include <RTAnySL/ShaderManagerRegistry.hpp>
#include <jitRT/llvmWrapper.h>
#include <sstream>

namespace RTAnySL
{

ShaderPtr loadShader(const std::string &fileName)
{
    const Utils::PathFinder &pf = Utils::getBitcodePathFinder();

    std::string filePath = pf.findPath(fileName);
    if (filePath.length() != 0)
    {
        ModuleAutoPtr module = jitRT::createModuleFromFile(filePath);

        if (module.get())
        {
            // try to find shader manager
            ShaderManagerPtr shaderManager =
                Singleton<ShaderManagerRegistry>::getInstance().
                findShaderManager(module.get());

            if (shaderManager == 0)
            {
                std::ostringstream oss;
                oss<<"No shader manager found for shader from \"";
                oss<<fileName<<"\" module";
                throw Exception(oss.str());
            }

            ShaderPtr shader = shaderManager->createShader(module.get());

            module.release();

            return shader;
        }
    }
    return ShaderPtr();
}

} // namespace RTAnySL
