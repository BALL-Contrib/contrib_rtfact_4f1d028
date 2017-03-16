/* 
 *  EnvLightGen.hpp
 *
 *  RTfact - Real-Time Ray Tracing Library
 *
 *  Copyright (C) 2011  Saarland University
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
 *  Created on: 2011-04-07 15:58:17 +0100
 *  Author(s): Ken Dahm
 */
 
#ifndef RTFACT__ENVMAPCONVERTER_HPP
#define RTFACT__ENVMAPCONVERTER_HPP

#include <RTfact/Utils/BBox.hpp>

RTFACT_NAMESPACE_BEGIN

class EnvMapConverter
{
    inline
    std::pair<float, float> intersectBBox(const Vec3f1& aOrigin,
                                          const Vec3f1& aDir,
                                          const Vec3f1& aMin,
                                          const Vec3f1& aMax) const
	{
        Vec3f1 invR = Vec3f1(1.0f, 1.0f, 1.0f) / aDir;
        Vec3f1 tbot = invR * (aMin - aOrigin);
        Vec3f1 ttop = invR * (aMax - aOrigin);
        Vec3f1 tmin = Vec3f1::min(ttop, tbot);
        Vec3f1 tmax = Vec3f1::max(ttop, tbot);
        return std::pair<float, float>(Math::max(Math::max(tmin.x, tmin.y), Math::max(tmin.x, tmin.z)),
					                   Math::min(Math::min(tmax.x, tmax.y), Math::min(tmax.x, tmax.z)));
	}

    template<class tImage2D>
    void convertDebCross(tImage2D& aSource) const
    {
        uint resX = aSource.getResX();
        uint resY = aSource.getResY();

        tImage2D tmp;
        tmp.init(aSource.getComponentCount(), resX, resY);
        for (uint y=0; y < resY; ++y)
        {
            for (uint x=0; x < resX; ++x)
            {
                for (uint i=0; i < aSource.getComponentCount(); ++i)
                {
                    tmp(x, y)[i] = aSource(x, aSource.getResY() - 1 - y)[i];
                }
            }
        }

        // Copy common part
        aSource.init(aSource.getComponentCount(), resY, resX);
        for (uint y=0; y < resX; ++y)
        {
            for (uint x=0; x < resX; ++x)
            {
                for (uint i=0; i < aSource.getComponentCount(); ++i)
                {
                    aSource(x, y)[i] = tmp(x, y)[i];
                }
            }
        }

        // Copy rest
        uint tileSize = Math::floorToInt(resX / 3.0f);
        for (uint y=0; y < tileSize; ++y)
        {
            for (uint x=0; x < tileSize; ++x)
            {
                for (uint i=0; i < aSource.getComponentCount(); ++i)
                {
                    aSource(x + 3 * tileSize, y + tileSize)[i] = tmp(2 * tileSize - 1 - x, 4 * tileSize - 1 - y)[i];
                }
            }
        }
    }

    template<class tImage2D>
    void convertCross(tImage2D& aSource) const
    {
        uint resX = aSource.getResX();
        uint resY = aSource.getResY();

        tImage2D tmp;
        tmp.init(aSource.getComponentCount(), aSource.getResX(), aSource.getResY());
        for (uint y=0; y < resY; ++y)
        {
            for (uint x=0; x < resX; ++x)
            {
                for (uint i=0; i < aSource.getComponentCount(); ++i)
                {
                    tmp(x, y)[i] = aSource(x, aSource.getResY() - 1 - y)[i];
                }
            }
        }

        aSource.init(tmp(0, 0), aSource.getComponentCount(), aSource.getResX(), aSource.getResY(), true);
    }

public:

    template<class tImage2D>
    bool convert(tImage2D& aTarget,
                 tImage2D& aSource) const
    {
        // Do not convert image, because it is
        // already in longitude/latitude format
        if (aSource.getResX() == 2 * aSource.getResY())
            return false;

        {
            FILE *fptr = fopen("C:/Users/kdahm/Desktop/test_original.ppm", "wb+");
            fprintf(fptr, "P6\n%d %d\n255\n", aSource.getResX(), aSource.getResY());
            for (uint y=0; y < aSource.getResY(); ++y)
            {
                for (uint x=0; x < aSource.getResX(); ++x)
                {
                    for (uint i=0; i < aSource.getComponentCount(); ++i)
                    {
                        unsigned char c = Math::clamp(static_cast<unsigned char>(aSource(x, y)[i] * 255.0f), (unsigned char)0, (unsigned char)255);
                        fwrite(&c, sizeof(unsigned char), 1, fptr);
                    }
                }
            }
            fclose(fptr);
        }

        // Convert Debevec's cross format
        if (aSource.getResX() > aSource.getResY())
            convertCross(aSource);
        else if (aSource.getResX() < aSource.getResY())
            convertDebCross(aSource);

        {
            FILE *fptr = fopen("C:/Users/kdahm/Desktop/test_converted.ppm", "wb+");
            fprintf(fptr, "P6\n%d %d\n255\n", aSource.getResX(), aSource.getResY());
            for (uint y=0; y < aSource.getResY(); ++y)
            {
                for (uint x=0; x < aSource.getResX(); ++x)
                {
                    for (uint i=0; i < aSource.getComponentCount(); ++i)
                    {
                        unsigned char c = Math::clamp(static_cast<unsigned char>(aSource(x, y)[i] * 255.0f), (unsigned char)0, (unsigned char)255);
                        fwrite(&c, sizeof(unsigned char), 1, fptr);
                    }
                }
            }
            fclose(fptr);
        }

        const uint resX = aSource.getResX();
        const uint resY = aSource.getResY();
        const uint componentCount = aSource.getComponentCount();
        const uint newResX = 2u * resX;
        const uint newResY = resX;
        aTarget.init(componentCount, newResX, newResY);

        for (uint y=0; y < newResY; ++y)
        {
            for (uint x=0; x < newResX; ++x)
            {
                const float phi = RTFACT_2_PI * (x + 0.5f) / newResX;
                const float theta = RTFACT_PI * (y + 0.5f) / newResY;
                const float sinTheta = Math::sin(theta);

                const Vec3f1 dir(
                    sinTheta * Math::cos(phi),
                    -Math::cos(theta),
                    sinTheta * Math::sin(phi));
            
                // Convert light probe image
                if (resX == resY)
                {
                    const float denom1 = Math::reciprocal(Math::sqrt(dir.xf * dir.xf + dir.yf * dir.yf));
                    const float denom = Math::min(denom1, RTFACT_INFINITY);

                    const float dirZClamped = Math::clamp(dir.zf, -1.0f, 1.0f);
                    const float numer = RTFACT_INV_PI * Math::acos(dirZClamped) * denom;

                    const float u = (dir.xf * numer + 1.f) * 0.5f;
                    const float v = (dir.yf * numer + 1.f) * 0.5f;

                    uint xidx = Math::floorToInt(u * (resX - 1));
                    uint yidx = Math::floorToInt(v * (resY - 1));
                    for (uint i=0; i < componentCount; ++i)
                        aTarget(x, y)[i] = aSource(xidx, yidx)[i];
                }
                // Convert light cross image
                else if (resX > resY)
                {
                    const float w = 1.0f / 4.0f;
                    const float h = 1.0f / 3.0f;
                    const float dx = Math::abs(dir.x), dy = Math::abs(dir.y), dz = Math::abs(dir.z);
                    float u, v;
                    if (dx > dy && dx > dz)
                    {
                        u = 0.5f * (dir.z / dx + 1.0f);
                        u = dir.x > 0.0f ? (1.0f - u) * w + 2.0f * w : u * w;
                        v = 0.5f * (dir.y / dx + 1.0f);
                        v = (1.0f - v) * h + h;
                    }
                    else if (dy > dx && dy > dz)
                    {
                        u = 0.5f * (dir.x / dy + 1.0f);
                        u = u * w + w;
                        v = 0.5f * (dir.z / dy + 1.0f);
                        v = dir.y > 0.0f ? v * h : (1.0f - v) * h + 2 * h;
                    }
                    else
                    {
                        u = 0.5f * (dir.x / dz + 1.0f);
                        u = dir.z > 0.0f ? u * w + w : (1.0f - u) * w + 3.0f * w;
                        v = 0.5f * (dir.y / dz + 1.0f);
                        v = (1.0f - v) * h + h;
                    }

                    uint xidx = Math::clamp(Math::floorToInt(u * (resX - 1)), 0, static_cast<int>(resX - 1));
                    uint yidx = Math::clamp(Math::floorToInt(v * (resY - 1)), 0, static_cast<int>(resY - 1));
                    for (uint i=0; i < componentCount; ++i)
                        aTarget(x, y)[i] = aSource(xidx, yidx)[i];
                }
            }
        }
        return true;
    }
};

RTFACT_NAMESPACE_END

#endif // RTFACT__ENVLIGHTGEN_HPP
