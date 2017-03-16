#ifndef RTANYSL_SHADERINSTANCEFWD_HPP_INCLUDED
#define RTANYSL_SHADERINSTANCEFWD_HPP_INCLUDED

#include <RTAnySL/Config/Common.hpp>
#include <boost/shared_ptr.hpp>

namespace RTAnySL
{

class ShaderInstance;
typedef boost::shared_ptr<ShaderInstance> ShaderInstancePtr;
struct ShaderInstanceContext;

} // namespace RTAnySL

#endif
