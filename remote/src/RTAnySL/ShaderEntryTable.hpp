#ifndef RTANYSL_SHADERENTRYTABLE_HPP_INCLUDED
#define RTANYSL_SHADERENTRYTABLE_HPP_INCLUDED

#include <RTAnySL/Config/Common.hpp>
#include <RTAnySL/ShaderEntryTableFwd.hpp>
#include <RTAnySL/Utils.hpp>
#include <vector>
#include <boost/utility.hpp>

#include <jitRT/llvmWrapper.h>

namespace RTAnySL
{

/// ShaderEntryTable represents a table of function pointers which represent
/// entry points into a shader, like shading function.
class RTANYSL_API ShaderEntryTable : private boost::noncopyable
{
public:

    struct Entry
    {
        void * funcPtr;
        llvm::Function * func;

        Entry() : funcPtr(0), func(0) { }
    };

    ShaderEntryTable(unsigned int size = 0, llvm::Module * module = 0);

    ~ShaderEntryTable();

    llvm::Module * getModule() const { return module; }

    /// setModule should be only called when table is empty
    void setModule(llvm::Module * module);

    llvm::ExecutionEngine * getEngine() const { return engine; }

    void create(unsigned int size, llvm::Module * module)
    {
        // TODO: should we clean-up table first ?
        setModule(module);
        create(size);
    }

    void create(unsigned int size);

    void clear();

    unsigned int size() const { return static_cast<int>(entries.size()); }

    bool empty() const { return entries.empty(); }

    const Entry & operator[](const unsigned int index) const
    {
        RTANYSL_ASSERT(index < entries.size(), "index is out of range");

        return entries[index];
    }

    Entry & operator[](const unsigned int index)
    {
        RTANYSL_ASSERT(index < entries.size(), "index is out of range");

        return entries[index];
    }

    /// initialize function entry by compiling given LLVM function
    void initEntry(const unsigned int index, llvm::Function * func);

    llvm::Function * getEntryFunc(const unsigned int index) const
    {
        RTANYSL_ASSERT(index < entries.size(), "index is out of range");

        return entries[index].func;
    }

    template <class T> T getEntryFuncPtr(const unsigned int index) const
    {
        RTANYSL_ASSERT(index < entries.size(), "index is out of range");

        return reinterpret_cast<T>(entries[index].funcPtr);
    }

private:
    llvm::Module * module;
    llvm::ExecutionEngine * engine;

    typedef std::vector<Entry> EntryVector;
    std::vector<Entry> entries;

    void freeMachineCodeForEntry(Entry &entry);
};

class RTANYSL_API ShaderEntryTableMaker
{
public:

    virtual ~ShaderEntryTableMaker() { }

    virtual void makeEntryTable(ShaderEntryTable *table) = 0;

};

} // namespace RTAnySL

#endif
