#include "Precompiled.hpp"

#define RTANYSL_LIB
#include <RTAnySL/Config/Common.hpp>

#include <RTAnySL/StaticInit.hpp>

namespace RTAnySL
{

// -- StaticInit --

StaticInit::~StaticInit()
{
}

void StaticInit::appendThisToInitList()
{
    StaticInitPtr &lastRef = getLastRef();

    if (lastRef == 0)
    {
        StaticInitPtr &firstRef = getFirstRef();
        assert(firstRef == 0);

        firstRef = this;
        lastRef = this;
    }
    else {
        lastRef->next = this;
        lastRef = this;
    }
}

void StaticInit::initialize()
{
    static bool initialized = false;

    if (initialized)
        return;
    else
        initialized = true;


    StaticInitPtr p = getFirstRef();
    while (p != 0)
    {
        if (p->initCB != 0)
            p->initCB();
        p = p->next;
    }
}

StaticInit::StaticInitPtr &StaticInit::getFirstRef()
{
    static StaticInitPtr firstRef = 0;

    return firstRef;
}

StaticInit::StaticInitPtr &StaticInit::getLastRef()
{
    static StaticInitPtr lastRef = 0;

    return lastRef;
}

} // namespace RTAnySL
