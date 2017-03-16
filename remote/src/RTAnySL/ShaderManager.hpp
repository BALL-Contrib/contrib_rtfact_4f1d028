#ifndef RTANYSL_SHADERMANAGER_HPP_INCLUDED
#define RTANYSL_SHADERMANAGER_HPP_INCLUDED

#include <RTAnySL/Config/Common.hpp>
#include <RTAnySL/ShaderManagerFwd.hpp>
#include <RTAnySL/ShaderFwd.hpp>
#include <jitRT/llvmWrapper.h>

namespace RTAnySL
{

class ShaderCompilerResult
{
public:

    ShaderCompilerResult() :
        errorCode(0), output()
    { }

    ShaderCompilerResult(int errorCode) :
        errorCode(errorCode), output()
    { }

    ShaderCompilerResult(int errorCode, const std::string &output) :
        errorCode(errorCode), output(output)
    { }

    void set(int _errorCode, const std::string &_output)
    {
        errorCode = _errorCode;
        output = _output;
    }

    /// when code is not 0 it is an error
    int getErrorCode() const { return errorCode; }

    void setErrorCode(int _errorCode)
    {
        errorCode = _errorCode;
    }

    void setOutput(const std::string &_output)
    {
        output = _output;
    }

    const std::string & getOutput() const { return output; }

private:
    int errorCode;
    std::string output;
};

/// This is base class for every shader manager
class RTANYSL_API ShaderManager
{
public:

    virtual ~ShaderManager() { }

    virtual std::string getShadingLanguage() const = 0;

    /// Returns true if the given LLVM module is supported by this
    ///  ShaderManager.
    /// This means that API-style / rendering engine of the module
    /// are compatible with this ShaderManager.
    virtual bool isModuleSupported(llvm::Module *module) const = 0;

    /// Creates shader from given LLVM module.
    /// If the module is not supported by this ShaderManager,
    /// exception is thrown.
    virtual ShaderPtr createShader(llvm::Module *module) = 0;

    /// Build new shader from specified language script
    /// If shaderCompilerResult is not NULL, the compiler error code,
    /// and output will be saved into it.
    virtual ShaderPtr buildShader(
            const std::string &shaderScript,
            ShaderCompilerResult * shaderCompilerResult = 0) = 0;

};

} // namespace RTAnySL

#endif
