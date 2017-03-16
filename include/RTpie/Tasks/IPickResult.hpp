/*
 * IPickResult.hpp
 *
 *  Created on: 06.01.2011
 *      Author: Boris Broenner
 */

#ifndef IPICKRESULT_H_
#define IPICKRESULT_H_

#include <RTpie/Base/IUnknown.hpp>
#include <RTpie/Scene/IMesh.hpp>
#include <RTpie/Scene/IInstance.hpp>

RTFACT_RTPIE_NAMESPACE_BEGIN

/**
 *
 *  The IPickResult interface provides the result set a picking operation task
 *  should provide. Hence it does not serve as a task directly but picking tasks
 *  should inherit from this class to conform to a common result interface.
 *
 *  The following functions should be supported to read the results:
 *  - HasResult   : Is there any result at all?
 *  - GetInstance : Get the hit instance of the picking operation given an IInstance instance
 *  - GetMesh     : Get the mesh of the picking operation given an IMesh interface instance
 *  - GetPosition : Get the hit point
 *  - GetNormal   : Get the normal at the hit point
 *
 */
struct IPickResult : public IUnknown
{
    /**
     * This always returns false, if the ray generation was not performed yet,
     * else it always returns true.
     * @param _retval contains true if ray generation has been performed.
     * @return HRESULT
     */
    virtual HRESULT HasResult(bool *_retval) = 0;


    /**
     * Access instance of the picking result
     * @param _retval IInstance interface pointer of the instance
     * @return HRESULT
     */
    virtual HRESULT GetInstance(IInstance **_retval) = 0;

    /**
     * Access mesh of the picking result
     * @param _retval IMesh interface pointer of the mesh
     * @return HRESULT
     */
    virtual HRESULT GetMesh(IMesh **_retval) = 0;

    /**
     * Access position of the picking result
     * @param _retval IMesh interface pointer of the mesh
     * @return HRESULT
     */
    virtual HRESULT GetPosition(float *aX, float *aY, float *aZ ) = 0;

    /**
     * Access normal of the picking result
     * @param _retval IMesh interface pointer of the mesh
     * @return HRESULT
     */
    virtual HRESULT GetNormal(float *aX, float *aY, float *aZ) = 0;

};

RTFACT_RTPIE_NAMESPACE_END

#endif /* IPICKRESULT_H_ */
