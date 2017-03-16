#include "Precompiled.hpp"

#define RTANYSL_LIB
#include <RTAnySL/Config/Common.hpp>

#include <RTAnySL/Exception.hpp>

namespace RTAnySL
{

// -- Exception --

Exception::Exception(const std::string &arg) :
    std::runtime_error(arg)
{
}

Exception::~Exception() throw()
{
}

} // namespace RTAnySL

