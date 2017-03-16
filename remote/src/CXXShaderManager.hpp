#ifndef CXXSHADERMANAGER_HPP_INCLUDED
#define CXXSHADERMANAGER_HPP_INCLUDED

#include "RTAnySLShaderBase.hpp"

#include <jitRT/llvmWrapper.h>

#include <RTremote/RTAppearance.hpp>
#include <RTremote/Parameterizable.hpp>
#include <RTAnySL/Shader.hpp>
#include <RTAnySL/ShaderInstance.hpp>
#include <RTAnySL/ShaderManager.hpp>
#include <RTAnySL/Utils.hpp>
#include <RTAnySL/ModuleAutoPtr.hpp>
#include <RTAnySL/ParameterCache.hpp>
#include <boost/enable_shared_from_this.hpp>

RTFACT_REMOTE_NAMESPACE_BEGIN

class CXXShaderInstance;
class CXXShader;
class CXXShaderManager;

typedef boost::shared_ptr<CXXShaderInstance> CXXShaderInstancePtr;
typedef boost::shared_ptr<CXXShader> CXXShaderPtr;
typedef boost::shared_ptr<CXXShaderManager> CXXShaderManagerPtr;

class CXXShaderInstance : public RTAnySL::ShaderInstance
{
    friend class CXXShader;
    friend class CXXShaderManager;
public:

    virtual ~CXXShaderInstance();

    CXXShaderPtr getCXXShader() const
    {
        return boost::static_pointer_cast<CXXShader>(getShader());
    }

protected:

    CXXShaderInstance(const RTAnySL::ShaderPtr &shader);

    // -- options support --

    /// returns empty value if option is not supported
    virtual RTAnySL::OptionValue getOptionValueImpl(
            const std::string &name) const;

    /// returns false if option is not supported
    virtual bool setOptionValueImpl(const std::string &name,
                                    const RTAnySL::OptionValue &value);

    virtual void getOptionsImpl(RTAnySL::OptionsList &options);

    // -- parameters support --

    /// returns -1 if there no parameter with given name
    virtual int getParamIndexImpl(const std::string &name) const;

    virtual RTAnySL::ParameterInfo getParamInfoImpl(int index) const;

    virtual void beginParamsImpl();

    virtual void endParamsImpl();

    /// returns false if there is no such parameter
    virtual bool getParamImpl(int index, void *data, int byteSize);

    /// returns false if there is no such parameter
    virtual bool setParamImpl(int index, const void *data, int byteSize);

    virtual void setParamsFromImpl(const RTAnySL::ShaderInstancePtr &source);

    virtual void getParamInfosImpl(RTAnySL::ParameterInfoVector &infos);

    virtual const RTAnySL::ShaderEntryTable & getEntryTableImpl();

    virtual const RTAnySL::ShaderInstanceContext & getContextImpl();

    virtual void * getGenericShadeFuncImpl();

private:

    struct CXXContext : public RTAnySL::ShaderInstanceContext
    {
        RTAnySL::ShaderEntryTable entryTable;
    };

    /// Because C++ shader currently represent shader parameters as global
    /// variables we need to store a cloned original shader module
    /// from CXXShader.
    RTAnySL::ModuleAutoPtr buildModule;

    bool packetize;
    bool specialize;
    bool needsUpdate;
    bool needsRebuild;

    RTAnySL::ParameterCache parameters;
    RTAnySL::ParameterInfoVector parameterInfos;
    CXXContext shaderContext;

    // required for temporary workaround
    void * dataEps;
    void * dataInf;

    /// init should be called in constructor
    void init();

    /// resetModule creates fresh clone of the shader module
    void resetModule();

    /// rebuildModule builds ready-to-shade module accordingly to the
    /// specified options
    void rebuildModule();

    /*
	 * specializes a global variable in the buildModule
	 */
    bool specializeParam(const std::string & paramName, const void * value);

    static void genericShadeFunc(
        const RTAnySL::ShaderInstanceContext * shaderInstanceContext,
        bool taCommonOrg,
        uint taSize,
        const void * aRayPacket,
        const void * aRayMask,
        void * aIntersection,
        void * aContext,
        void * oResult);

};

class CXXShader : public RTAnySL::Shader,
                  public boost::enable_shared_from_this<CXXShader>
{
    friend class CXXShaderManager;
public:

    virtual ~CXXShader();

    virtual RTAnySL::ShaderInstancePtr createInstance(
        const RTAnySL::OptionsList *options = 0);

protected:

    CXXShader(const RTAnySL::ShaderManagerPtr &shaderManager,
              const RTAnySL::ShaderPtr &parentShader,
              llvm::Module * module);

};

class CXXShaderManager : public RTAnySL::ShaderManager,
                         public boost::enable_shared_from_this<CXXShaderManager>
{
    friend class CXXShader;
    friend class CXXShaderInstance;
public:

    CXXShaderManager();

    virtual ~CXXShaderManager();

    virtual std::string getShadingLanguage() const;

    virtual bool isModuleSupported(llvm::Module *module) const;

    virtual RTAnySL::ShaderPtr createShader(llvm::Module *module);

    virtual RTAnySL::ShaderPtr buildShader(
            const std::string &shaderScript,
            RTAnySL::ShaderCompilerResult * result = 0);

    static bool buildCXXShader(
        const std::string &shaderScript,
        std::string &bcModuleFileName,
        RTAnySL::ShaderCompilerResult * result);

private:
    RTAnySL::ModuleAutoPtr shaderGlueModule;
    RTAnySL::ModuleAutoPtr scalarWrapperModule;

    RTAnySL::ShaderInstancePtr createShaderInstance(
        const CXXShaderPtr &shader,
        const RTAnySL::OptionsList *options);

};

RTFACT_REMOTE_NAMESPACE_END

#endif /* CXXSHADERMANAGER_HPP_INCLUDED */
