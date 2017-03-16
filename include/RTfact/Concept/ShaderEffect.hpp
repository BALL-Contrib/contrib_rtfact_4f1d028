/* 
 *  ShaderEffect.hpp
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
 *  Created on: 2010-06-11 12:35:28 +0200
 *  Author(s): Petr Kmoch, Lukas Marsalek
 */
 
 #ifndef RTFACT__SHADEREFFECT_HPP
#define RTFACT__SHADEREFFECT_HPP

#include <RTfact/Config/Common.hpp>

RTFACT_NAMESPACE_BEGIN

template <
    typename tParameterId,
    typename tParameters>
class ShaderEffect
{
public:
    /*!
     * Returns stored value of aParameter.
     * \return true iff aParameter belongs to this effect and value was retrieved successfully
     */
    template <
        typename tParameterValue>
    const bool getParameter(
        const tParameterId aParameter,
        tParameterValue &aValue) const;
    
    /*!
     * Sets the stored value of aParameter. Does not update GPU state.
     * \return true iff aParameter belongs to this effect and value was stored successfully
     */
    template <
        typename tParameterValue>
    const bool setParameter(
        const tParameterId aParameter,
        const tParameterValue &aValue);

    /*!
     * Updates GPU value of aParameter according to value stored internally.
     * \return true iff aParameter belongs to this effect
     */
    const bool updateParameter(
        const tParameters &aParameters,
        const tParameterId aParameter) const;
    
    /*!
     * Updates GPU values of all parameters of this effect.
     */
    void updateParameters(
        const tParameters &aParameters) const;
    
    /*!
     * \return Set of all parameters belonging to this effect.
     */
    const std::set<tParameterId>& getSettableParameters() const;

    void preRender(
        const tParameters &aParameters);

    void prePass(
        const size_t aPass,
        const tParameters &aParameters);

    void postPass(
        const size_t aPass,
        const tParameters &aParameters);

    void postRender(
        const tParameters &aParameters);
    
    /*!
     * True iff the modeling class has a nonempty preRender() method
     */
    static const bool mHasPreRender;
    /*!
     * A bit combination, such that bit 2^N is set iff prePass(N) does something.
     */
    static const umask mHasPrePass;
    /*!
     * A bit combination, such that bit 2^N is set iff postPass(N) does something.
     */
    static const umask mHasPostPass;
    /*!
     * True iff the modeling class has a nonempty postRender() method
     */
    static const bool mHasPostRender;
    
    /*!
     * A bit combination, such that bit 2^N is set iff the GPU's color framebuffer should be cleared before pass N
     */
    static const umask mPrePassClearColor;
    
    /*!
     * Returns a unique name of the effect.
     */
    static const std::string getName();
    /*!
     * Returns a path to the effect's file.
     */
    static const std::string getFileName();
    /*!
     * Returns compiler arguments to pass to effect compiler when compiling this effect.
     */
    static const char ** const getCompileArgs();
};

RTFACT_NAMESPACE_END

#endif // RTFACT__SHADEREFFECT_HPP
