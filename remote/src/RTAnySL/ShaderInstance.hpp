#ifndef RTANYSL_SHADERINSTANCE_HPP_INCLUDED
#define RTANYSL_SHADERINSTANCE_HPP_INCLUDED

#include <RTAnySL/Config/Common.hpp>
#include <RTAnySL/ShaderInstanceFwd.hpp>
#include <RTAnySL/ShaderFwd.hpp>
#include <RTAnySL/ShaderManagerFwd.hpp>
#include <RTAnySL/Options.hpp>
#include <RTAnySL/ParameterInfo.hpp>
#include <RTAnySL/ShaderEntryTable.hpp>

namespace RTAnySL
{

struct ShaderInstanceContext
{
};

/// ShaderInstance represents a shader with a set of specified shader parameters
class RTANYSL_API ShaderInstance
{
public:

    virtual ~ShaderInstance();

    const ShaderPtr &getShader() const { return shader; }

    // -- options support --

    bool hasOption(const std::string &name) const
    {
        return !getOptionValue(name).isEmpty();
    }

    /// returns empty value if option is not supported
    OptionValue getOptionValue(const std::string &name) const
    {
        return getOptionValueImpl(name);
    }

    /// returns false if option is not supported
    bool setOptionValue(const std::string &name, const OptionValue &value)
    {
        return setOptionValueImpl(name, value);
    }

    bool setOption(const Option &option)
    {
        return setOptionValue(option.getName(), option.getValue());
    }

    void getOptions(OptionsList &options)
    {
        getOptionsImpl(options);
    }

    void setOptions(const OptionsList &options)
    {
        for (OptionsList::const_iterator it = options.begin();
             it != options.end(); ++it)
        {
            setOption(*it);
        }
    }

    // -- parameters support --

    /// Check if shader parameter with given name does exist.
    bool hasParam(const std::string &name) const
    {
        return getParamIndexImpl(name) != -1;
    }

    int getParamIndex(const std::string &name) const
    {
        return getParamIndexImpl(name);
    }

    ParameterInfo getParamInfo(int index) const
    {
        return getParamInfoImpl(index);
    }

    bool getParam(int index, void *data, int byteSize)
    {
        return getParamImpl(index, data, byteSize);
    }

    bool getParam(const std::string &name, void *data, int byteSize)
    {
        int index = getParamIndex(name);
        return index != -1 ? getParamImpl(index, data, byteSize) : false;
    }

    void setParamsFrom(const ShaderInstancePtr &source)
    {
        setParamsFromImpl(source);
    }

    bool setParam(int index, const void *data, int byteSize)
    {
        return setParamImpl(index, data, byteSize);
    }

    bool setParam(const std::string &name, const void *data, int byteSize)
    {
        int index = getParamIndex(name);
        return index != -1 ? setParamImpl(index, data, byteSize) : false;
    }

    // This function needs to be called before modification of shader parameters
    // or build options starts
    void beginParams()
    {
        beginParamsImpl();
    }

    // This function needs to be called after modification of shader parameters
    // or build options is finished.
    void endParams()
    {
        endParamsImpl();
    }

    /// Store parameter infos into infos vector
    void getParamInfos(ParameterInfoVector &infos)
    {
        getParamInfosImpl(infos);
    }

    /// Returns reference to the current entry table of the shader
    /// (e.g. table of shading functions).
    const ShaderEntryTable & getEntryTable()
    {
        return getEntryTableImpl();
    }

    const ShaderInstanceContext & getContext()
    {
        return getContextImpl();
    }

    /// Returns generic shade function. Its signature is specific
    /// to the renderer and will be casted there.
    /// Normally one of the arguments will be the entry table provided
    /// by the getEntryTable call.
    /// We need generic shade function additionally to the entry table
    /// as different shading languages will need to put different structures
    /// on the stack. The returned shade function will do exactly this
    /// and provide to the renderer the same interface independently of
    /// the internally used shading language.
    template <class T> T getGenericShadeFunc()
    {
        return reinterpret_cast<T>(getGenericShadeFuncImpl());
    }

protected:

    ShaderInstance();

    ShaderInstance(const ShaderPtr &shader);

    // -- options support --

    /// returns empty value if option is not supported
    virtual OptionValue getOptionValueImpl(const std::string &name) const = 0;

    /// returns false if option is not supported
    virtual bool setOptionValueImpl(const std::string &name,
                                    const OptionValue &value) = 0;

    virtual void getOptionsImpl(OptionsList &options) = 0;

    // -- parameters support --

    /// returns -1 if there no parameter with given name
    virtual int getParamIndexImpl(const std::string &name) const = 0;

    virtual ParameterInfo getParamInfoImpl(int index) const = 0;

    virtual void beginParamsImpl() = 0;

    virtual void endParamsImpl() = 0;

    /// returns false if there is no such parameter
    virtual bool getParamImpl(int index, void *data, int byteSize) = 0;

    /// returns false if there is no such parameter
    virtual bool setParamImpl(int index, const void *data, int byteSize) = 0;

    virtual void setParamsFromImpl(const ShaderInstancePtr &source) = 0;

    virtual void getParamInfosImpl(ParameterInfoVector &infos) = 0;

    virtual const ShaderEntryTable & getEntryTableImpl() = 0;

    virtual const ShaderInstanceContext & getContextImpl() = 0;

    virtual void * getGenericShadeFuncImpl() = 0;

private:
    ShaderPtr shader;
};

} // namespace RTAnySL

#endif
