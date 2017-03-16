#ifndef RTANYSL_SHADER_HPP_INCLUDED
#define RTANYSL_SHADER_HPP_INCLUDED

#include <RTAnySL/Config/Common.hpp>
#include <RTAnySL/ShaderFwd.hpp>
#include <RTAnySL/ShaderInstanceFwd.hpp>
#include <RTAnySL/ShaderManagerFwd.hpp>
#include <RTAnySL/ModuleAutoPtr.hpp>
#include <RTAnySL/Options.hpp>
#include <jitRT/llvmWrapper.h>

namespace RTAnySL
{

class RTANYSL_API Shader
{
public:

    /// Default destructor will delete stored LLVM module.
    virtual ~Shader();

    const ShaderManagerPtr &getManager() const { return shaderManager; }

    llvm::Module * getModule() const { return module.get(); }
    const ModuleAutoPtr& getModuleAutoPtr() const { return module; }

    /// Resets the module. Note that when a different module is stored
    /// in shader it will be deleted.
    /// Only for internal use.
    void setModule(llvm::Module *module);

    /// Returns internal module and releases ownership of it.
    llvm::Module * releaseModule();

    /// Check if shader parameter with given name does exist.
    /// Default implementation will check for global variable in the LLVM
    /// module.
    virtual bool hasParam(const std::string &name) const;

    /// If options is not NULL, they will be processed
    /// and used to customize the shader instance.
    virtual ShaderInstancePtr createInstance(const OptionsList *options = 0) = 0;

protected:

    Shader();

    /// Note that shader will take ownership of the provided module,
    /// so the default destructor will destroy the module.
    Shader(const ShaderManagerPtr &shaderManager,
           const ShaderPtr &parentShader,
           llvm::Module * module);

private:
    ShaderManagerPtr shaderManager;
    ShaderPtr parentShader;
    ModuleAutoPtr module;
};

} // namespace RTAnySL

#endif
