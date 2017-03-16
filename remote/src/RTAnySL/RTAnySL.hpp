#ifndef RTANYSL_RTANYSL_HPP_INCLUDED
#define RTANYSL_RTANYSL_HPP_INCLUDED

#include <RTAnySL/Config/Common.hpp>
#include <RTAnySL/ShaderFwd.hpp>
#include <RTAnySL/LibraryInit.hpp>

namespace RTAnySL
{

RTANYSL_API ShaderPtr loadShader(const std::string &fileName);

} // namespace RTAnySL

#endif
