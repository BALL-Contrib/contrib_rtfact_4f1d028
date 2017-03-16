#ifndef RTANYSL_PARAMETERCACHE_HPP_INCLUDED
#define RTANYSL_PARAMETERCACHE_HPP_INCLUDED

#include <RTAnySL/Config/Common.hpp>
#include <RTAnySL/Utils.hpp>
#include <jitRT/llvmWrapper.h>
#include <boost/pointer_cast.hpp>
#include <map>
#include <vector>
#include <cstring>
#include <iostream>
#include <iomanip>

namespace RTAnySL
{

#define ANYSL_GLOBAL_SIZE_LIMIT_BYTES 2048

class Parameter
{
public:

    Parameter(int size) :
        byteSize(size),
        value(0)
    {
        RTANYSL_ASSERT(byteSize > 0, "Invalid type size given to Parameter constructor");
        RTANYSL_ASSERT(byteSize < ANYSL_GLOBAL_SIZE_LIMIT_BYTES, "parameter type exceeds internal size limit");

        value = new char [byteSize];
        ::memset(value, 0, byteSize);
    }

    Parameter(const Parameter &other) :
        byteSize(other.byteSize),
        value(0)
    {
        value = new char[byteSize];
        copyValueFrom(other);
    }

    ~Parameter()
    {
        delete [] value;
    }

    bool canCopyValueFrom(const Parameter &other)
    {
        return other.byteSize == byteSize;
    }

    void copyValueFrom(const Parameter &other)
    {
        copyValueFrom(other.value);
    }

    void copyValueFrom(const void * sourceValue)
    {
        ::memcpy(value, static_cast<const char*>(sourceValue), byteSize);
    }

    void copyValueTo(void * dest)
    {
        ::memcpy(dest, value, byteSize);
    }

    bool isValueEqual(const void * cmpValue)
    {
        const char * bytes = value;
        const char * other = static_cast<const char*>(cmpValue);

        for (int i = 0; i < byteSize;++i)
        {
            if (bytes[i] != other[i])
                return false;
        }
        return true;
    }

    int getSize() const
    {
        return byteSize;
    }

    void * getValuePtr() const
    {
        return static_cast<void*>(value);
    }

    bool operator==(const Parameter &other)
    {
        return (other.byteSize == byteSize &&
                isValueEqual(other.getValuePtr()));
    }

    void operator=(const Parameter &other)
    {
        if (other.byteSize > byteSize)
        {
            delete [] value;
            value = new char [other.byteSize];
        }
        byteSize = other.byteSize;
        copyValueFrom(other);
    }

private:
    int byteSize;
    char *value;
};

class ParameterCache
{
public:
	typedef std::map<std::string, int> IndexMap;
    typedef std::vector<Parameter*> ParameterList;

    typedef IndexMap::iterator IndexMapIterator;
    typedef IndexMap::const_iterator IndexMapConstIterator;

    ParameterCache()
    {
    }

    ~ParameterCache()
    {
        clear();
    }

    unsigned int size() const { return static_cast<int>(parameters.size()); }

    /// Create parameter entry for every global variable with external linkage.
    int initFromModule(llvm::Module * shaderModule)
    {
        RTANYSL_ASSERT(shaderModule != 0, "shader was not properly initialized. shaderModule is NULL");

        clear();

        std::vector<unsigned> valueByteSizes;
        int numParams =
            jitRT::mapGlobalValues(shaderModule, indexMap, valueByteSizes);

        typedef std::vector<unsigned>::const_iterator Iter;
        for (Iter it = valueByteSizes.begin(); it != valueByteSizes.end(); ++it)
        {
            parameters.push_back(new Parameter(*it));
        }

        return numParams;
    }

    /// Creates zero initializers for global variables with the names of
    /// parameters. This is required before global variables with external
    /// linkage can be accessed.
    void createZeroInitializersForGlobalVariables(
        llvm::Module * shaderModule) const
    {
        for (IndexMapConstIterator it = indexMap.begin();
             it != indexMap.end(); ++it)
        {
            jitRT::setGlobalVariableZero(shaderModule, it->first);
        }
    }

    void setModuleGlobalVariables(llvm::ExecutionEngine *engine,
                                  llvm::Module * shaderModule) const
    {
        for (IndexMapConstIterator it = indexMap.begin();
             it != indexMap.end(); ++it)
        {
            std::string name = it->first;
            int index = it->second;

            llvm::GlobalVariable * gv =
                jitRT::getGlobalVariable(shaderModule, name);

            RTANYSL_ASSERT(gv != 0, "global variable is NULL");

            // There seems to be a bug in JIT which does not always allocate
            // memory for the global variable when getOrEmitGlobalVariable
            // function is used.
            // So we use jitRT::getPointerToGlobal instead.
            // Note that cast to llvm::GlobalValue * is allowed
            // as GlobalVariable is inherited from GlobalValue.
            // void * dest = jitRT::getOrEmitGlobalVariable(engine, gv);
            void *dest = jitRT::getPointerToGlobal(engine,
                                                   (llvm::GlobalValue*)(gv));

            RTANYSL_ASSERT(dest != 0, "cannot get or emit global variable");
            RTANYSL_ASSERT(parameters[index] != 0, "parameters[index] is NULL");

            RTANYSL_DEBUG("Parameters::setModuleGlobalVariables: SET PARAM "<<it->first<<" ptr="<<dest);

            parameters[index]->copyValueTo(dest);
        }
    }

    void getModuleGlobalVariables(llvm::ExecutionEngine *engine,
                                  llvm::Module * shaderModule) const
    {
        for (IndexMapConstIterator it = indexMap.begin();
             it != indexMap.end(); ++it)
        {
            std::string name = it->first;
            int index = it->second;

            llvm::GlobalVariable * gv =
                jitRT::getGlobalVariable(shaderModule, name);

            RTANYSL_ASSERT(gv != 0, "global variable is NULL");

            // There seems to be a bug in JIT which does not always allocate
            // memory for the global variable when getOrEmitGlobalVariable
            // function is used.
            // So we use jitRT::getPointerToGlobal instead.
            // Note that cast to llvm::GlobalValue * is allowed
            // as GlobalVariable is inherited from GlobalValue.
            // void * dest = jitRT::getOrEmitGlobalVariable(engine, gv);
            void * src = jitRT::getPointerToGlobal(engine,
                                                   (llvm::GlobalValue*)(gv));

            RTANYSL_ASSERT(src != 0, "cannot get or emit global variable");
            RTANYSL_ASSERT(parameters[index] != 0, "parameters[index] is NULL");

            RTANYSL_DEBUG("Parameters::getModuleGlobalVariables: GET param "<<it->first<<" ptr="<<src);

            parameters[index]->copyValueFrom(src);
        }
    }

    /// Returns index of parameter with specified name or -1 when there
    /// is no such parameter.
    int getParameterIndex(const std::string &name) const
    {
        IndexMapConstIterator it = indexMap.find(name);
        return it != indexMap.end() ? it->second : -1;
    }

    Parameter * getParameter(const std::string &name) const
    {
        IndexMapConstIterator it = indexMap.find(name);
        if (it != indexMap.end())
        {
            RTANYSL_ASSERT(it->second >= 0 && it->second < parameters.size(), "index out of range");

            return parameters[it->second];
        }
        return 0;
    }

    Parameter * getParameter(int index) const
    {
        RTANYSL_ASSERT(index >= 0 && index < parameters.size(), "index out of range");

        return parameters[index];
    }

    void * getParameterValuePtr(int index) const
    {
        RTANYSL_ASSERT(index >= 0 && index < parameters.size(), "index out of range");

        Parameter *p = parameters[index];
        return p ? p->getValuePtr() : 0;
    }

    IndexMapConstIterator indexMapBegin() const
    {
        return indexMap.begin();
    }

    IndexMapConstIterator indexMapEnd() const
    {
        return indexMap.end();
    }

    void swap(ParameterCache &other)
    {
        std::swap(indexMap, other.indexMap);
        std::swap(parameters, other.parameters);
    }

    void clear()
    {
        indexMap.clear();
        for (ParameterList::iterator it = parameters.begin();
             it != parameters.end(); ++it)
            delete *it;
        parameters.clear();
    }

    void copyValuesFrom(const ParameterCache &other)
    {
        for (IndexMapConstIterator oit = other.indexMapBegin();
             oit != other.indexMapEnd(); ++oit)
        {
            // get parameter by name
            Parameter *param = getParameter(oit->first);
            // get parameter by index
            Parameter *otherParam = other.getParameter(oit->second);

            RTANYSL_ASSERT(otherParam != 0, "parameter to copy from is NULL");

            if (param && param->canCopyValueFrom(*otherParam))
            {
                param->copyValueFrom(*otherParam);
            }
        }
    }

    template <class T> static void printData(
        std::ostream &out,
        const char * name,
        const void *data,
        const size_t dataSize)
    {
        if (dataSize % sizeof(T) == 0)
        {
            const unsigned int numElements = static_cast<unsigned int>(dataSize / sizeof(T));
            out<<name<<"["<<numElements<<"]=";
            for (unsigned int i = 0; i < numElements; ++i)
            {
                out<<(static_cast<const T*>(data)[i]);
                if (i != numElements-1)
                {
                    out<<' ';
                }
            }
        }
    }

    void dumpParameterData(int index, std::ostream &out) const
    {
        Parameter * p = getParameter(index);
        RTANYSL_ASSERT(p != 0, "parameter is NULL");
        out<<" ptr="<<p->getValuePtr();
        out<<" size="<<p->getSize();

        out<<" ";
        printData<float>(out, "float", p->getValuePtr(), p->getSize());
        out<<" ";
        printData<int>(out, "int", p->getValuePtr(), p->getSize());

        out<<std::hex<<std::setfill('0')<<std::setw(2);

        out<<" value=";

        for (int i = 0; i < p->getSize(); ++i)
        {
            out<<int(static_cast<unsigned char*>(p->getValuePtr())[i])<<' ';
        }
        out<<std::dec<<std::endl;
    }

    void dumpParameter(int index, std::ostream &out)
    {
        for (IndexMapConstIterator it = indexMapBegin();
             it != indexMapEnd(); ++it)
        {
            if (it->second == index)
            {
                out<<"param name=\""<<it->first<<"\" index="<<it->second;
                dumpParameterData(it->second, out);
                return;
            }
        }
    }

    void dump(std::ostream &out) const
    {
        out<<"ParameterCache: num parameters="<<parameters.size()<<std::endl;
        out<<"index size="<<indexMap.size()<<std::endl;
        out<<"-------------------------------"<<std::endl;
        for (IndexMapConstIterator it = indexMapBegin();
             it != indexMapEnd(); ++it)
        {
            out<<"param name=\""<<it->first<<"\" index="<<it->second;
            Parameter * p = getParameter(it->second);
            dumpParameterData(it->second, out);
        }
        out<<"-------------------------------"<<std::endl;
    }

private:
	IndexMap indexMap;
	ParameterList parameters;
};

} // namespace RTAnySL

#endif
