#ifndef RTFACT_REMOTE__RTANYSLSHADERBASE_HPP
#define RTFACT_REMOTE__RTANYSLSHADERBASE_HPP

#include <RTfact/Concept/Appearance/SurfaceShader.hpp>
#include "RTAnySLShaderAPI.hpp"

#include <jitRT/llvmWrapper.h>

#include <RTAnySL/ShaderInstanceFwd.hpp>

RTFACT_REMOTE_NAMESPACE_BEGIN

//#define RTANYSL_DONT_RENDER 1

#define RTANYSL_RAY_MAX_DEPTH 10

typedef void (*RTfactGenericShadeFunc)(
    const RTAnySL::ShaderInstanceContext * shaderInstanceContext,
    bool taCommonOrg,
    uint taSize,
    const void * aRayPacket,
    const void * aRayMask,
    void * aIntersection,
    void * aContext,
    void * oResult);

template<class tContext>
class RTAnySLShaderBase :
    public SurfaceShader<tContext>
{
private:
    const RTAnySL::ShaderInstanceContext * shaderInstanceContextPtr;
    RTfactGenericShadeFunc shadeFunc;

public:

    RTAnySLShaderBase() : shaderInstanceContextPtr(0), shadeFunc(0)
    { }

    ~RTAnySLShaderBase()
    { }

    void setShaderInstanceContextPtr(const RTAnySL::ShaderInstanceContext *_context)
    {
        shaderInstanceContextPtr = _context;
    }

    const RTAnySL::ShaderInstanceContext * getShaderInstanceContextPtr() const
    {
        return shaderInstanceContextPtr;
    }

    void setShadeFunc(RTfactGenericShadeFunc _shadeFunc)
    {
        shadeFunc = _shadeFunc;
    }

    RTfactGenericShadeFunc getShadeFunc() const
    {
        return shadeFunc;
    }

    template<
        bool taCommonOrg,
        bool taStoreAlpha,
        uint taSize>
    RTFACT_APPEARANCE_INLINE void shade(
        const RayPacket<taSize>& aRayPacket,
        const ActiveMask<taSize>& aRayMask,
        SurfaceIntersection<taSize>& aIntersection,
        tContext& aContext,
        Vec3f<taSize>& oResult,
        Packet<taSize, float>* oResultAlpha = RTFACT_NULL) const
    {
        typedef Packet<taSize, float>         t_FPacket;
        typedef typename t_FPacket::Container t_FPacketC;

#ifndef RTANYSL_DONT_RENDER
        if (shadeFunc && shaderInstanceContextPtr)
        {
            // TODO : remove when all shader languages handle max depth
            //        properly.
            if (aRayPacket.depth > RTANYSL_RAY_MAX_DEPTH)
            {
                oResult = Vec3f<taSize>::C_0_0_0();
                return;
            }

            RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aRayMask)
            {
                if(RTFACT_CHECK_UMASK(tContext::Scene::PROPERTIES, SceneProperties::INVERT_NORMAL))
                {
                    aIntersection.normal(i) =
                        (Math::dot(aIntersection.normal(i), aRayPacket.dir(i)) >
                            t_FPacketC::C_0()).iif(
                                -aIntersection.normal(i),
                                aIntersection.normal(i));
                }
            }

            (*shadeFunc)(shaderInstanceContextPtr,
                         taCommonOrg, taSize,
                         &aRayPacket, &aRayMask, &aIntersection,
                         &aContext, &oResult);
        }
        else
            oResult = aRayMask.iif(Vec3f<taSize>::C_1_0_0, oResult);
#endif
    }

    template<
        uint taSize>
    RTFACT_APPEARANCE_INLINE void attenuateLight(
        const ActiveMask<taSize>& aMask,
        SurfaceIntersection<taSize>& aIntersection,
        Mask<taSize>& oMask,
        Vec3f<taSize>& oIntensity) const
    {
         //const t_MaskC falseC = t_MaskC::C_FALSE();

        RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aMask)
        {
            oMask(i) &= ~aMask(i);
        }
    }

    #include RTFACT_SURFACE_SHADER_VIRTUAL_METHODS
};



RTFACT_REMOTE_NAMESPACE_END

#endif /* RTFACT_REMOTE__RTANYSLSHADERBASE_HPP */
