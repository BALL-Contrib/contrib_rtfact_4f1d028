/*
 * IRayPickTask.hpp
 *
 *  Created on: 06.01.2011
 *      Author: Boris Broenner
 */

#ifndef IRAYPICKTASK_H_
#define IRAYPICKTASK_H_

#include <RTpie/Config/Common.hpp>
#include <RTpie/Tasks/IPickResult.hpp>

RTFACT_RTPIE_NAMESPACE_BEGIN

#define RTPIE_IRAYPICKTASK_ID \
       { 0xe4e1041d, 0x4bdd, 0x422a, { 0x97, 0x95, 0x4f, 0xf9, 0x6a, 0x16, 0xda, 0x80 } }

/**
 *
 *  The IRayPickTask Interface is used to configure and execute a
 *  picking operation using a ray.
 *
 *  Ray picking needs the following to be configured:
 *  - a Camera Configuration (via ICamera)
 *  - a ray
 *
 *  After configuration call Run() to perform the generation.
 *
 *  Afterwards the functions defined in the IPickResult interface can be
 *  used to obtain the result.
 *
 */
struct IRayPickTask : public IPickResult
{
    RTPIE_DECLARE_STATIC_IID_ACCESSOR(RTPIE_IRAYPICKTASK_ID)

    /**
     *  Shoot the given ray in the scene and compute pick result
     *  @param origX the x-component of the ray origin
     *  @param origY the y-component of the ray origin
     *  @param origZ the z-component of the ray origin
     *  @param dirX the x-component of the ray direction
     *  @param dirY the y-component of the ray direction
     *  @param dirZ the z-component of the ray direction
     *  @return HRESULT
     */
    virtual HRESULT RayPick(const float origX, const float origY, const float origZ,
                         const float dirX, const float dirY, const float dirZ) = 0;
};

RTFACT_RTPIE_NAMESPACE_END

#endif /* IRAYPICKTASK_H_ */
