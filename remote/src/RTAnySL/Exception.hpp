#ifndef RTANYSL_EXCEPTION_HPP_INCLUDED
#define RTANYSL_EXCEPTION_HPP_INCLUDED

#include <RTAnySL/Config/Common.hpp>
#include <stdexcept>

namespace RTAnySL
{

class RTANYSL_API Exception : public std::runtime_error
{
public:

    explicit Exception(const std::string &arg);

    virtual ~Exception() throw();
};

} // namespace RTAnySL

#endif
