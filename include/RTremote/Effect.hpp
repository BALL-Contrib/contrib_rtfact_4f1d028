/* 
 *  Effect.hpp
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
 
 #ifndef RTFACT_REMOTE__EFFECT_HPP
#define RTFACT_REMOTE__EFFECT_HPP


#include <RTremote/Config/Common.hpp>

#include <RTremote/ReferenceCounted.hpp>
#include <RTremote/Parameterizable.hpp>

RTFACT_POSTPROCESSING_NAMESPACE_BEGIN

class EffectPimpl;
class PostprocessorPimpl;


class RTFACT_REMOTE_API Effect :
    public ReferenceCounted
{
private:
    EffectPimpl *pimpl;
    size_t mIdxEffect;

public:
    Effect(
        PostprocessorPimpl &aPostprocessor,
        const size_t aIdxEffect,
        const std::string &aName);
    
    virtual ~Effect();
    
    const EffectHandle getNextEffect() const;
    
    const std::string getName() const;
    
    bool isEnabled() const;
    
    void setEnabled(
        const bool aEnabled=true);
};

RTFACT_POSTPROCESSING_NAMESPACE_END

#endif  //RTFACT_REMOTE__EFFECT_HPP
