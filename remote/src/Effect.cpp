/* 
 *  Effect.cpp
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
 *  Author(s): Lukas Marsalek, Felix Klein
 */
 
 #include "Precompiled.hpp"

#include <RTfactRemoteConfig.hpp>
#include <RTfact/Config/Common.hpp>

#define RTFACT_REMOTE_LIB

#include <RTremote/Effect.hpp>
#include "RTPrivatePostprocessor.hpp"

RTFACT_POSTPROCESSING_NAMESPACE_BEGIN

//---------------------------------------------------------------------
// Effect Definition
//---------------------------------------------------------------------

Effect::Effect(
    PostprocessorPimpl &aPostprocessor,
    const size_t aIdxEffect,
    const std::string &aName)
:
    pimpl(new EffectPimpl(aPostprocessor,aName)),
    mIdxEffect(aIdxEffect)
{}

Effect::~Effect()
{
    delete pimpl;
}

const EffectHandle Effect::getNextEffect() const
{
    if (
        mIdxEffect+1<pimpl->mPostprocessor.mEffects.size())
    {
        return pimpl->mPostprocessor.mEffects[mIdxEffect+1];
    }
    else
    {
        return NULL;
    }
}

const std::string Effect::getName() const
{
    return pimpl->mName;
}

bool Effect::isEnabled() const
{
    return pimpl->mPostprocessor.mPostprocessor.isEffectEnabled(mIdxEffect);
}

void Effect::setEnabled(
    const bool aEnabled)
{
    pimpl->mPostprocessor.mPostprocessor.setEffectEnabled(mIdxEffect,aEnabled);
}

//---------------------------------------------------------------------
// EffectPimpl Definition
//---------------------------------------------------------------------

EffectPimpl::EffectPimpl(
    PostprocessorPimpl &aPostprocessor,
    const std::string &aName)
:
    mPostprocessor(aPostprocessor),
    mName(aName)
{}

RTFACT_POSTPROCESSING_NAMESPACE_END
