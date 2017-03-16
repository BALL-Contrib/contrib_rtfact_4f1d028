/*
 *  EnvironmentLight.hpp
 *
 *  RTfact - Real-Time Ray Tracing Library
 *
 *  Copyright (C) 2009  Saarland University
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
 *  Created on: 2011-01-05 17:30:26 +0200
 *  Author(s): Ken Patrik Dahm
 */

#ifndef RTFACT__ENVIRONENTLIGHT_HPP
#define RTFACT__ENVIRONENTLIGHT_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Concept/Light.hpp>
#include <RTfact/Model/LightAttenuation/QuadraticLightAttenuation.hpp>
#include <RTfact/Model/Image/BasicImage2D.hpp>
#include <RTfact/Utils/Distribution.hpp>
#include <RTfact/Utils/MipMap.hpp>
#include <RTfact/Utils/Transform.hpp>
#include <RTfact/Utils/EnvMapConverter.hpp>
#include <RTfact/Utils/ImageTraitUtils.hpp>

RTFACT_NAMESPACE_BEGIN

template<
    class tImage2D>
class EnvironmentLight :
    public Light
{
    BoundingSphere mBSphere;
    MipMap         mMap;
    Distribution2D mCDF;
    float          mCDFNormX, mCDFNormY;
    float          mCDFScaleX, mCDFScaleY;
    Transform      mTransform;
    Vec3f<1>       mRadianceScale;

    void init()
    {
        mTransform = Transform::identity();

        RTFACT_LIGHT_ASSERT(mMap.getResX() > 0 && mMap.getResY() > 0);

        const uint cdfResX = static_cast<uint>(mMap.getOrigResX() * mCDFScaleX);
        const uint cdfResY = static_cast<uint>(mMap.getOrigResY() * mCDFScaleY);

        mCDFNormX = Math::reciprocal(float(cdfResX));
        mCDFNormY = Math::reciprocal(float(cdfResY));

        const float filterWidth = Math::min(mCDFNormX, mCDFNormY);
        const float sinThetaNorm = RTFACT_PI * mCDFNormY;

        mCDF.init(cdfResX, cdfResY);

        for(uint y = 0; y < cdfResY; ++y)
        {
            const float sinTheta = Math::sin(float(y + 0.5f) * sinThetaNorm);
            const float yf = float(y) * mCDFNormY;

            for(uint x = 0; x < cdfResX; ++x)
            {
                const Vec2f1 coords(float(x) * mCDFNormX, yf);
                Vec3f1 color;

                mMap.lookup<true, false>(coords, &color, RTFACT_NULL, filterWidth);

                mCDF(x, y) = color.luminance() * sinTheta;
            }
        }

        mCDF.build();
    }

    void samplePrivate(
        const Vec2f1& aSamples,
        Vec3f1& oRadiance,
        Vec3f1& oDirectionToLight,
        float& oPDF) const
    {
        Vec2f1 coords = mCDF.sampleContinuous(aSamples, oPDF);

        const float phi = RTFACT_2_PI * coords.x;
        const float theta = RTFACT_PI * coords.y;
        const float sinTheta = Math::sin(theta);

        oPDF /= RTFACT_2_PI_PI * sinTheta;

        const Vec3f1 sampledDir(
            sinTheta * Math::sin(phi),
            sinTheta * Math::cos(phi),
            -Math::cos(theta));

        oDirectionToLight = mTransform.applyToVectorAffine(sampledDir);

        mMap.lookup<true, false>(coords, &oRadiance, RTFACT_NULL);

        oRadiance *= mRadianceScale;
    }

public:
    RTFACT_LIGHT_INLINE EnvironmentLight(
        const Vec3f<1>& aRadianceScale,
        const tImage2D *aImage = RTFACT_NULL,
        const float aCDFScaleX = 1.0f,
        const float aCDFScaleY = 1.0f) 
        : Light(Light::CASTS_SHADOWS),
          mRadianceScale(aRadianceScale),
          mCDFScaleX(aCDFScaleX),
          mCDFScaleY(aCDFScaleY) 
    {
        Math::seedRandom();

        if (aImage == RTFACT_NULL)
        {
            BasicImage2D<float> fImage;
            fImage.init(3, 1, 1);
            fImage(0, 0)[0] = aRadianceScale.xf;
            fImage(0, 0)[1] = aRadianceScale.yf;
            fImage(0, 0)[2] = aRadianceScale.zf;
            mMap.init(fImage, WRAP_REPEAT);
        }
        else
        {
            // Convert to float image
            typedef typename tImage2D::Component tSrcComponent;
            BasicImage2D<float> fImage;
            fImage.init(aImage->getComponentCount(), aImage->getResX(), aImage->getResY());
            for (uint y=0; y < aImage->getResY(); ++y)
            {
                for (uint x=0; x < aImage->getResX(); ++x)
                {
                    float *component = fImage(x, y);
                    const tSrcComponent *srcComponent = (*aImage)(x, y);
                    for (uint i=0; i < aImage->getComponentCount(); ++i)
                    {
                        component[i] = ImageComponentConverter<tSrcComponent, float>::convert(srcComponent[i]);
                    }
                }
            }

            // Convert image to LL
            BasicImage2D<float> ll;
            EnvMapConverter mapConverter;
            if (mapConverter.convert(ll, fImage))
                mMap.init(ll, WRAP_REPEAT);
            else
                 mMap.init(fImage, WRAP_REPEAT);
        }

        init();
    }

    RTFACT_LIGHT_INLINE void setBoundingSphere(const BoundingSphere& aBSphere)
    {
        mBSphere = aBSphere;
    }

    template<
        uint taSize>
    RTFACT_LIGHT_INLINE void illuminatePoint(
        const Vec2f<taSize>& aPositionSamples,
        const Vec3f<taSize>& aPoint,
        const ActiveMask<taSize>& aMask,
        Vec3f<taSize>& oDirection,
        Packet<taSize, float>& oDistance,
        Vec3f<taSize>& oRadiance,
        Vec3f<1> *) const
    {
        typedef Packet<taSize, float>        t_Packet;
        typedef typename t_Packet::Container t_PacketC;
        typedef Vec3f<taSize>                t_Vec3f;
        typedef typename t_Vec3f::Container  t_Vec3fC;

        oDistance = t_Packet::replicate(RTFACT_INFINITY);

        RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aMask)
        {
            t_PacketC dx, dy, dz, rx, ry, rz;
            for (uint k=0; k < t_Vec3fC::SIZE; ++k)
            {
                float pdf;
                Vec3f<1> dirToLight, radiance;
                samplePrivate(aPositionSamples(i)[k], radiance, dirToLight, pdf);
    
                radiance /= pdf;

                dx[k] = dirToLight.xf;
                dy[k] = dirToLight.yf;
                dz[k] = dirToLight.zf;
                rx[k] = radiance.xf;
                ry[k] = radiance.yf;
                rz[k] = radiance.zf;
            }

            oDirection(i) = t_Vec3fC(dx, dy, dz);
            oRadiance(i) = t_Vec3fC(rx, ry, rz);
        }
    }

    template<
        uint taSize>
    RTFACT_LIGHT_INLINE void sample(
        const Vec2f<taSize>& aPositionSamples,
        const Vec2f<taSize>& aDirectionSamples,
        const ActiveMask<taSize>& aMask,
        Vec3f<taSize>& oPosition,
        Vec3f<taSize>& oDirection,
        Vec3f<taSize>& oRadiance,
        Packet<taSize, float>& oPdf) const
    {
        typedef Packet<taSize, float>         t_Packet;
        typedef typename t_Packet::Container  t_PacketC;
        typedef Vec2f<taSize>                 t_Vec2f;
        typedef typename t_Vec2f::Container   t_Vec2fC;
        typedef Vec3f<taSize>                 t_Vec3f;
        typedef typename t_Vec3f::Container   t_Vec3fC;

        /*

        RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aMask)
        {
            const t_PacketC r = Math::sqrt(t_PacketC::replicate(aPositionSamples(i).x));
            const t_PacketC u = t_PacketC::replicate(1) - r;
            const t_PacketC v = t_PacketC::replicate(aPositionSamples(i).y) * r;
            const t_PacketC w = t_PacketC::replicate(1) - u - v;

            oPosition(i) = u * vertex0C + v * vertex1C + w * vertex2C;
            oDirection(i) = Math::cosineHemisphere(aDirectionSamples(i), &oPdf(i));
            oPdf(i) *= mInvArea;
            oRadiance(i) = oDirection(i).z * mIntensity;
            oDirection(i) = Math::transformToWorld(oDirection(i), normalC, binormalC, tangentC);
        }*/
        /*float dirPDF, posPDF;

        samplePrivate(aDirectionSamples, oRadiance, oDirection, dirPDF);

        const Vec2f1 pos = Math::concentricDisk(aPositionSamples, &posPDF);

        posPDF /= aWorldBounds.radius * aWorldBounds.radius;

        oPDF = dirPDF * posPDF;

        Vec3f1 binormal, tangent;
        Math::getFrame(oDirection, binormal, tangent);

        oLightPoint =
            aWorldBounds.center + aWorldBounds.radius *
            (pos.x * binormal + pos.y * tangent + oDirection);

        oDirection = -oDirection;*/

        Packet<taSize, float> pdf;
        RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aMask)
        {
            t_PacketC dx, dy, dz, rx, ry, rz;
            for (uint k=0; k < t_Vec3fC::SIZE; ++k)
            {
                //float pdf;
                Vec3f<1> dirToLight, radiance;
                samplePrivate(aDirectionSamples(i)[k], radiance, dirToLight, pdf(i)[k]);
    
                //radiance /= pdf;

                dx[k] = dirToLight.xf;
                dy[k] = dirToLight.yf;
                dz[k] = dirToLight.zf;
                rx[k] = radiance.xf;
                ry[k] = radiance.yf;
                rz[k] = radiance.zf;
            }

            oDirection(i) = t_Vec3fC(dx, dy, dz);
            oRadiance(i) = t_Vec3fC(rx, ry, rz);
        }

        t_Vec3f binormal, tangent;
        Math::getFrame(oDirection, binormal, tangent);

        const t_Vec3fC bsCenterC = t_Vec3fC::replicate(mBSphere.center);
        const t_PacketC bsRadius = t_PacketC::replicate(mBSphere.radius);
        const t_PacketC invRadSq = t_PacketC::replicate(1.0f / (mBSphere.radius * mBSphere.radius));

        //oRadiance = t_Vec3f::replicate(mIntensity);
        //oDirection = t_Vec3f::replicate(-mDirection);
        RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aMask)
        {
            t_Vec2fC pos = Math::concentricDisk(aPositionSamples(i), &oPdf(i));
            oPdf(i) *= invRadSq * pdf(i);
            oPosition(i) = bsCenterC + bsRadius * (pos.x * binormal(i) + pos.y * tangent(i) + oDirection(i));
            oDirection(i) = -oDirection(i);
        }
    }

    RTFACT_LIGHT_INLINE const Vec3f<1> getTotalPower() const
    {
        float power = mCDF.getTotal() / (mCDF.getResX() * mCDF.getResY()) *
                      RTFACT_PI * mBSphere.radius * mBSphere.radius;
        return Vec3f<1>(power, power, power);
    }

    #include RTFACT_LIGHT_VIRTUAL_METHODS
};

RTFACT_NAMESPACE_END

#endif // RTFACT__TRIANGLELIGHT_HPP
