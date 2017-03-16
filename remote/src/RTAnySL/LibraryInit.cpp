#include "Precompiled.hpp"

#define RTANYSL_LIB
#define RTFACT_REMOTE_LIB
#include <RTAnySL/Config/Common.hpp>

#include <RTAnySL/LibraryInit.hpp>
#include <RTAnySL/StaticInit.hpp>
#include <RTAnySL/ShaderManagerRegistry.hpp>

namespace RTAnySL
{

using namespace std;

int LibraryInit::counter = 0;

void LibraryInit::initializeImpl()
{
    StaticInit::initialize();
}

void LibraryInit::shutdownImpl()
{
    RTAnySL::Singleton<RTAnySL::ShaderManagerRegistry>::getInstance().clear();
}

} // namespace RTSG
