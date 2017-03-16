#ifndef RTFACT__ASYMMETRICPINHOLECAMERA_HPP
#define RTFACT__ASYMMETRICPINHOLECAMERA_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Concept/Camera/RayTracingCamera.hpp>

RTFACT_NAMESPACE_BEGIN

class AsymmetricPinholeCamera :
    public RayTracingCamera
{    
    float mAspectRatio;
    float mFrustumLeft;
    float mFrustumRight;
    float mFrustumTop;
    float mFrustumBottom;
    float mFrustumNearPlane;
    float mFrustumFarPlane;

    // precomputed terms for efficiency

    Vec3f1 mDeltaX;
    Vec3f1 mDeltaY;
    Vec3f1 mNearPlaneBottomLeft;

    Vec3fC mPosC;
    Vec3fC mDeltaXC;
    Vec3fC mDeltaYC;
    Vec3fC mNearPlaneBottomLeftC;

public:

    RTFACT_SIMD_ALIGNED_NEW_OPERATORS

    struct RTFACT_ALIGN_SIMD Params
        : public Camera::Params
    {
        RTFACT_SIMD_ALIGNED_NEW_OPERATORS

        float frustumLeft;
        float frustumRight;
        float frustumTop;
        float frustumBottom;
        float frustumNearPlane;
        float frustumFarPlane;
        bool useNonSquarePixels;

        Params() :
            frustumLeft(-1.f),
            frustumRight(1.f),
            frustumTop(1.f),
            frustumBottom(-1.f),
            frustumNearPlane(1.f),
            frustumFarPlane(Packet<1, float>::C_INFINITY),
            useNonSquarePixels(false)
        {}
    };

    void setParams(
        Params& aParams)
    {
        RayTracingCamera::setParams(aParams);
        
        mFrustumLeft = aParams.frustumLeft;
        mFrustumRight = aParams.frustumRight;
        mFrustumTop = aParams.frustumTop;
        mFrustumBottom = aParams.frustumBottom;
        mFrustumNearPlane = aParams.frustumNearPlane;
        mFrustumFarPlane = aParams.frustumFarPlane;
        
        //If we have opening angle set, ignore the frustum values        
        if(mOpeningAngle != 0.f)
        {
            mFrustumNearPlane = 1.f / Math::tan(Math::radians(mOpeningAngle) * 0.5f);

            mFrustumLeft = -1.f;
            mFrustumRight = 1.f;
            mFrustumTop = 1.f;
            mFrustumBottom = -1.f;
            mFrustumFarPlane = Packet<1, float>::C_INFINITY;
        }
                
        //Directly map frustum specification to screen space. This is required for example
        //for projectors, which do not have square pixels
        if(aParams.useNonSquarePixels)
        {
            mAspectRatio = 1.f;            
        }
        //If standard square pixels are to be used, rescale the frustum by the ratio of width/height
        else 
        {            
            mAspectRatio = mResXf / mResYf;
        }        
        
        float frustumWidth = mFrustumRight - mFrustumLeft;
        float frustumHeight = mFrustumTop - mFrustumBottom;

        mDeltaX = (frustumWidth * mAspectRatio) / mResX * mXAxis;
        mDeltaY = frustumHeight / mResY * mYAxis;
        mNearPlaneBottomLeft = mAspectRatio * mFrustumLeft * mXAxis + mFrustumBottom * mYAxis + mFrustumNearPlane * mZAxis;        

        mPosC = Vec3f<4>::replicate(mPos);
        mDeltaXC = Vec3f<4>::replicate(mDeltaX);
        mDeltaYC = Vec3f<4>::replicate(mDeltaY);
        mNearPlaneBottomLeftC = Vec3f<4>::replicate(mNearPlaneBottomLeft);
    }

    template<
        uint taSize>
    RTFACT_CAMERA_INLINE const RayPacket<taSize> generateRay(
        PixelSampler::Sample<taSize>& aSample) const
    {
        typedef typename Packet<taSize, float>::Container t_PacketC;

        const t_PacketC zero = t_PacketC::C_0();

        RayPacket<taSize> ret;

        //ret.org(0) = mPosC;

        ret.frustum.primaryRays = true;

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)
        {
            ret.org(i) = mPosC;
            ret.dir(i) = aSample.imageX(i) * mDeltaXC +
                         aSample.imageY(i) * mDeltaYC + mNearPlaneBottomLeftC;
            ret.dir(i).setNormalized();
            ret.invDir(i) = ret.dir(i).reciprocal();
            ret.tMin(i) = zero;
            //ToDo: Add far plane computation
            ret.tMax(i) = t_PacketC::C_INFINITY;
        }

        return ret;
    }
};

template<>
RTFACT_CAMERA_INLINE const RayPacket<1> AsymmetricPinholeCamera::generateRay<1>(
    PixelSampler::Sample<1>& aSample) const
{   
    RayPacket<1> ret;

    ret.org = mPos;

    ret.dir = aSample.imageX * mDeltaX +
              aSample.imageY * mDeltaY + mNearPlaneBottomLeft;
    ret.dir.setNormalized();
    ret.invDir = ret.dir.reciprocal();

    ret.tMin = Packet<1, float>::C_RAY_EPS;
    //ToDo: Add far plane computation
    ret.tMax = Packet<1, float>::C_INFINITY;

    return ret;
}

RTFACT_NAMESPACE_END

#endif // RTFACT__ASYMMETRICPINHOLECAMERA_HPP
