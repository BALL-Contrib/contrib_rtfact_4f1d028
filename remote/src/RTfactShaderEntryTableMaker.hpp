#ifndef RTFACT_REMOTE_RTANYSLENTRYTABLEMAKER_HPP_INCLUDED
#define RTFACT_REMOTE_RTANYSLENTRYTABLEMAKER_HPP_INCLUDED

#include <RTfact/Concept/Appearance/SurfaceShader.hpp>
#include "RTAnySLShaderAPI.hpp"

#include <jitRT/llvmWrapper.h>

#include <RTAnySL/ShaderEntryTable.hpp>
#include <RTAnySL/Singleton.hpp>

RTFACT_REMOTE_NAMESPACE_BEGIN

class RTfactShaderEntryTableMaker : public RTAnySL::ShaderEntryTableMaker
{
    RTANYSL_DECLARE_DEFAULT_SINGLETON(RTfactShaderEntryTableMaker)
public:

    enum {
        NUM_PACKET_SIZES = 5,
        // we have two states of common origin argument (true/false)
        // so need to multiply by 2
        ENTRY_TABLE_SIZE = NUM_PACKET_SIZES * 2
    };

    RTfactShaderEntryTableMaker();

    ~RTfactShaderEntryTableMaker();

    virtual void makeEntryTable(
        RTAnySL::ShaderEntryTable *table);

    static int getShadeFuncIndex(bool commonOrigin, int packetSize);

};

RTFACT_REMOTE_NAMESPACE_END

#endif
