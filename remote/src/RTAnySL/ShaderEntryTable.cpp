#include "Precompiled.hpp"

#define RTANYSL_LIB
#include <RTAnySL/Config/Common.hpp>

#include <RTAnySL/ShaderEntryTable.hpp>
#include <RTAnySL/Exception.hpp>
#include <jitRT/llvmWrapper.h>
#include <sstream>

namespace RTAnySL
{

ShaderEntryTable::ShaderEntryTable(unsigned int size, llvm::Module * module) :
    module(module), engine(0), entries(size)
{
    if (module != 0)
        engine = jitRT::createExecutionEngine(module);
}

ShaderEntryTable::~ShaderEntryTable()
{
    clear();
    // TODO : destroy engine ?
}

void ShaderEntryTable::setModule(llvm::Module * _module)
{
    module = _module;
    // ??? should we throw exception when table is not empty
    // TODO : destroy engine ?
    if (module != 0)
        engine = jitRT::createExecutionEngine(module);
}

void ShaderEntryTable::create(unsigned int size)
{
    entries.resize(size);
}

void ShaderEntryTable::clear()
{
    if (module)
    {
        for (EntryVector::iterator it = entries.begin();
             it != entries.end(); ++it)
        {
            freeMachineCodeForEntry(*it);
        }
    }
    entries.clear();
}

void ShaderEntryTable::initEntry(
    const unsigned int index, llvm::Function * _func)
{
    RTANYSL_ASSERT(index < entries.size(), "index is out of range");

    freeMachineCodeForEntry(entries[index]);

    entries[index].func = _func;
    entries[index].funcPtr = jitRT::getPointerToFunction(engine, _func);
}

void ShaderEntryTable::freeMachineCodeForEntry(Entry &entry)
{
    if (entry.func)
    {
        entry.funcPtr = 0;
        jitRT::freeMachineCodeForFunction(module, entry.func);
    }
}

} // namespace RTAnySL
