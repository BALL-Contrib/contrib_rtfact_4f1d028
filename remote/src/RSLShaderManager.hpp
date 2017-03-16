#ifndef RSLSHADERMANAGER_HPP_INCLUDED
#define RSLSHADERMANAGER_HPP_INCLUDED

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

class RSLShaderInstance;
class RSLShader;
class RSLShaderManager;

typedef boost::shared_ptr<RSLShaderInstance> RSLShaderInstancePtr;
typedef boost::shared_ptr<RSLShader> RSLShaderPtr;
typedef boost::shared_ptr<RSLShaderManager> RSLShaderManagerPtr;

class RSLShaderInstance : public RTAnySL::ShaderInstance
{
    friend class RSLShader;
    friend class RSLShaderManager;
public:

    virtual ~RSLShaderInstance();

    RSLShaderPtr getRSLShader() const
    {
        return boost::static_pointer_cast<RSLShader>(getShader());
    }

protected:

    RSLShaderInstance(const RTAnySL::ShaderPtr &shader);

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

    RTAnySL::ParameterInfo getParamInfoImpl(int index) const;

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

    struct RSLSIContext : public RTAnySL::ShaderInstanceContext
    {
        RTAnySL::ShaderEntryTable entryTable;
        void * paramPtr;   /// pointer to RSL parameter table
        void * texturePtr; /// pointer to RSL texture table

        RSLSIContext() : entryTable(), paramPtr(0), texturePtr(0) { }
    };

    /// Because C++ shader currently represent shader parameters as global
    /// variables we need to store a cloned original shader module
    /// from CXXShader.
    RTAnySL::ModuleAutoPtr buildModulePtr;

    bool packetize;
    bool specialize;
    bool needsUpdate;
    bool needsRebuild;

    llvm::TargetData * targetData;
    llvm::ExecutionEngine * engine;
    llvm::Function * initFn;

    RTAnySL::ParameterCache parameters;
    RTAnySL::ParameterInfoVector parameterInfos; /// textures occupy last numTexture elements
    std::vector<size_t> parameterOffsets; // stores offsets into parameter table
    unsigned int numTextures;
    
	RSLSIContext shaderContext;

    /// init should be called in constructor
    void init();

    /// resetModule creates fresh clone of the shader module
    void resetModule();

    /// rebuildModule builds ready-to-shade module accordingly to the
    /// specified options
    void rebuildModule(bool initParamsAndTextures = false);


    void * allocateAndInitShaderParams();

    void freeShaderParams(void * paramPtr);

    void * allocateAndInitTextureTable();

    void freeTextureTable(void * texturePtr);

    /// Returns index of the parameter with specified name or -1 if
    /// no such parameter was found
    int findParameterByName(const std::string &name) const;

    const RTAnySL::ShaderEntryTable & getEntryTable() const
    {
        return shaderContext.entryTable;
    }

    unsigned int getNumParameters() const
    {
        return static_cast<int>(parameterInfos.size());
    }

    const RTAnySL::ParameterInfo & getParameterInfo(unsigned int index) const
    {
        RTANYSL_ASSERT(index >= 0 && index < parameterInfos.size(), "index is out of range");
        return parameterInfos[index];
    }

    size_t getParameterOffset(unsigned int index) const
    {
        RTANYSL_ASSERT(index >= 0 && index < parameterOffsets.size(), "index is out of range");
        return parameterOffsets[index];
    }

    /// returns number of RenderMan (non texture) parameters
    unsigned int getNumRSLParameters() const
    {
        return static_cast<int>(parameterOffsets.size());
    }

    unsigned int getNumTextures() const
    {
        return numTextures;
    }

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

class RSLShader : public RTAnySL::Shader,
                  public boost::enable_shared_from_this<RSLShader>
{
    friend class RSLShaderManager;
public:

    virtual ~RSLShader();

    virtual RTAnySL::ShaderInstancePtr createInstance(
        const RTAnySL::OptionsList *options = 0);

protected:

    RSLShader(const RTAnySL::ShaderManagerPtr &shaderManager,
              const RTAnySL::ShaderPtr &parentShader,
              llvm::Module * module);

};

class RSLShaderManager : public RTAnySL::ShaderManager,
                         public boost::enable_shared_from_this<RSLShaderManager>
{
    friend class RSLShader;
    friend class RSLShaderInstance;
public:

    RSLShaderManager();

    virtual ~RSLShaderManager();

    virtual std::string getShadingLanguage() const;

    virtual bool isModuleSupported(llvm::Module *module) const;

    virtual RTAnySL::ShaderPtr createShader(llvm::Module *module);

    virtual RTAnySL::ShaderPtr buildShader(
            const std::string &shaderScript,
            RTAnySL::ShaderCompilerResult * result = 0);

    static bool buildRSLShader(
        const std::string &shaderScript,
        std::string &bcModuleFileName,
        RTAnySL::ShaderCompilerResult * result);

private:
    RTAnySL::ModuleAutoPtr runtimeModule;
    RTAnySL::ModuleAutoPtr glueModule;
    RTAnySL::ModuleAutoPtr packetGlueModule;

    RTAnySL::ShaderInstancePtr createShaderInstance(
        const RSLShaderPtr &shader,
        const RTAnySL::OptionsList *options);

};

RTFACT_REMOTE_NAMESPACE_END

#endif /* RSLSHADERMANAGER_HPP_INCLUDED */
