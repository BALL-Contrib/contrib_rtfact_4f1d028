#ifndef RTANYSL_MODULEAUTOPTR_HPP_INCLUDED
#define RTANYSL_MODULEAUTOPTR_HPP_INCLUDED

#include <RTAnySL/Config/Common.hpp>
#include <jitRT/llvmWrapper.h>

namespace RTAnySL
{


/// Automatically destroy llvm::Module when exiting from scope.
/// Modeled after std::auto_ptr
class ModuleAutoPtr
{

    ///  A wrapper class to provide ModuleAutoPtr with reference semantics.
    ///  Modeled after auto_ptr_ref<T>
    struct ModuleAutoPtrRef
    {
        llvm::Module * module;

        explicit ModuleAutoPtrRef(llvm::Module * module): module(module) { }
    };

public:

    ModuleAutoPtr(llvm::Module * module = 0) throw() : module(module) { }

    ModuleAutoPtr(ModuleAutoPtr & moduleAutoPtr) throw() :
        module(moduleAutoPtr.release())
    { }

    ModuleAutoPtr(ModuleAutoPtrRef ref) throw() : module(ref.module) { }

    ModuleAutoPtr & operator=(ModuleAutoPtr & moduleAutoPtr) throw()
    {
        reset(moduleAutoPtr.release());
        return *this;
    }

    ModuleAutoPtr & operator=(ModuleAutoPtrRef moduleAutoPtrRef) throw()
    {
        if (moduleAutoPtrRef.module != this->get())
        {
			//FIXME: temporarily disabled deletion to allow for recompliation
			//       with enabled packetization
            //jitRT::deleteModule(module);
            module = moduleAutoPtrRef.module;
        }
        return *this;
    }

    ~ModuleAutoPtr()
    {
		//FIXME: temporarily disabled deletion to allow for recompliation
		//       with enabled packetization
        //jitRT::deleteModule(module);
    }

    operator ModuleAutoPtrRef() throw()
    {
        return ModuleAutoPtrRef(this->release());
    }

    llvm::Module & operator*() const throw()
    {
        return *module;
    }

    llvm::Module * operator->() const throw()
    {
        return module;
    }

    llvm::Module * get() const throw() { return module; }

    llvm::Module * release() throw()
    {
        llvm::Module * tmp = module;
        module = 0;
        return tmp;
    }

    void reset(llvm::Module * _module = 0) throw()
    {
        if (_module != module)
        {
			//FIXME: temporarily disabled deletion to allow for recompliation
			//       with enabled packetization
            //jitRT::deleteModule(module);
            module = _module;
        }
    }

    void cloneFrom(llvm::Module * _module)
    {
        reset(jitRT::cloneModule(_module));
    }

    void cloneFrom(const ModuleAutoPtr & moduleAutoPtr)
    {
        cloneFrom(moduleAutoPtr.get());
    }

    ModuleAutoPtr clone() const
    {
        return ModuleAutoPtr(jitRT::cloneModule(module));
    }


private:
    llvm::Module * module;
};

} // namespace RTAnySL

#endif
