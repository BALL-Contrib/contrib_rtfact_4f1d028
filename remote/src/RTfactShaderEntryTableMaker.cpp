#include "Precompiled.hpp"

#define RTFACT_REMOTE_LIB
#define RTANYSL_LIB
#include <RTfactRemoteConfig.hpp>
#include <RTfact/Config/Common.hpp>
#include <RTremote/Config/Common.hpp>
#include "RTfactShaderEntryTableMaker.hpp"
#include "RTremotePrivateConfig.hpp"

RTFACT_REMOTE_NAMESPACE_BEGIN

RTANYSL_DEFINE_SINGLETON(RTfactShaderEntryTableMaker)

namespace
{

inline int getPacketSize(int packetSizeIndex)
{
    switch (packetSizeIndex)
    {
        case 0: return 1;
        case 1: return 4;
        case 2: return 16;
        case 3: return 64;
        case 4: return 256;
        default: return -1;
    }
}

inline std::string getShadeFuncName(bool commonOrigin, int packetSize)
{
    char buffer[512];
    //sprintf(buffer, "void generic_masked_shade<%s, %iu>",
    sprintf(buffer, "generic_masked_shade_%s%i",
            commonOrigin ? "true" : "false", packetSize);
    return std::string(buffer);
}

inline llvm::Function * assembleShadeFunc(bool commonOrg, int packetSize,
                                          llvm::Module *module)
{
    std::string shadeFuncName = getShadeFuncName(commonOrg, packetSize);

    llvm::Function * shadeFunc = jitRT::getFunction(shadeFuncName, module);
    //llvm::Function * shadeFunc = jitRT::getMangledFunction(shadeFuncName, module);

    if (!shadeFunc)
    {

        std::cerr<<"Could not find a c++ function called "
                 <<shadeFuncName<<std::endl;
        exit(-1);
    }

    //std::string name = jitRT::GCCdemangle(jitRT::getFunctionName(shadeFunc));
    //printf("compiling %s\n", name.c_str());
    printf("compiling %s\n", shadeFuncName.c_str());

    jitRT::optimizeFunction(shadeFunc);

    return shadeFunc;
}

} // unnamed namespace

RTfactShaderEntryTableMaker::RTfactShaderEntryTableMaker()
{
}

RTfactShaderEntryTableMaker::~RTfactShaderEntryTableMaker()
{
}

void RTfactShaderEntryTableMaker::makeEntryTable(
    RTAnySL::ShaderEntryTable *table)
{
    RTANYSL_ASSERT(table != 0, "table is NULL");
    RTANYSL_ASSERT(table->getModule() != 0, "module is NULL");
    RTANYSL_ASSERT(table->getEngine() != 0, "engine is NULL");

    table->create(RTfactShaderEntryTableMaker::ENTRY_TABLE_SIZE);

    for (unsigned int i = 0; i < RTfactShaderEntryTableMaker::NUM_PACKET_SIZES; i++)
    {
        const int packetSize = getPacketSize(i);

        table->initEntry(getShadeFuncIndex(true, packetSize),
                         assembleShadeFunc(true, packetSize,
                                           table->getModule()));

        table->initEntry(getShadeFuncIndex(false, packetSize),
                         assembleShadeFunc(false, packetSize,
                                           table->getModule()));
    }
}

int RTfactShaderEntryTableMaker::getShadeFuncIndex(bool commonOrigin, int packetSize)
{
    int sizeIndex;
    switch (packetSize)
    {
        case 1:   sizeIndex = 0; break;
        case 4:   sizeIndex = 1; break;
        case 16:  sizeIndex = 2; break;
        case 64:  sizeIndex = 3; break;
        case 256: sizeIndex = 4; break;
        default:
            printf("unknown size %i\n", packetSize);
            return -1;
    };

    return (commonOrigin ? 1 : 0) * RTfactShaderEntryTableMaker::NUM_PACKET_SIZES + sizeIndex;
}

RTFACT_REMOTE_NAMESPACE_END
