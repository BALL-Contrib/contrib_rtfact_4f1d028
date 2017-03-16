/* 
 *  PostprocessorParameter.hpp
 *
 *  RTfact - Real-Time Ray Tracing Library
 *
 *  Copyright (C) 2010  Saarland University
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  Created on: 2010-06-11 12:47:29 +0200
 *  Author(s): Lukas Marsalek
 */
 
 #ifndef RTFACT_REMOTE__POSTPROCESSORPARAMETER_HPP
#define RTFACT_REMOTE__POSTPROCESSORPARAMETER_HPP


#include <RTremote/Config/Common.hpp>

#include <boost/preprocessor/arithmetic/dec.hpp>
#include <boost/preprocessor/iteration/iterate.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/seq/elem.hpp>
#include <boost/preprocessor/seq/size.hpp>

#include <RTremote/ReferenceCounted.hpp>
#include <RTremote/Parameterizable.hpp>

RTFACT_POSTPROCESSING_NAMESPACE_BEGIN

class PostprocessorParameterPimpl;
class PostprocessorPimpl;


#define RTFACT_POSTPROCESSORPARAMETER_TYPES (unsigned int)(int)(float)(double)


class RTFACT_REMOTE_API PostprocessorParameter :
    public ReferenceCounted
{
    friend class PostprocessorPimpl;

private:
    PostprocessorParameterPimpl *pimpl;
    size_t mIdxParameter;

public:
    PostprocessorParameter(
        PostprocessorPimpl &aPostprocessor,
        const size_t aIdxParameter);
    
    virtual ~PostprocessorParameter();
    
    const PostprocessorParameterHandle getNextParameter() const;
    
    const std::string getName() const;
    
    size_t getValueComponentCount() const;
    
    #define BOOST_PP_ITERATION_PARAMS_1 (3,(0,BOOST_PP_DEC(BOOST_PP_SEQ_SIZE(RTFACT_POSTPROCESSORPARAMETER_TYPES)),<RTremote/Detail/PostprocessorParameterValue.hpp>))
    #include BOOST_PP_ITERATE()
};

RTFACT_POSTPROCESSING_NAMESPACE_END

#endif  //RTFACT_REMOTE__POSTPROCESSORPARAMETER_HPP
