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

#ifndef RTFACT__ENVLIGHTGEN_HPP
#define RTFACT__ENVLIGHTGEN_HPP

#include <RTfact/Utils/Containers/Vector.hpp>

RTFACT_NAMESPACE_BEGIN

class EnvLightGen
{
public:
    struct EnvironmentLight
    {
        Vec3f1 direction;
        Vec3f1 intensity;
    };

private:
    void computeRegion(Vector<EnvironmentLight>& lightSources,
                       float *mono,
                       const Vec3f1 *pixels,
                       const uint x0,
                       const uint y0,
                       const uint x1,
                       const uint y1,
                       const uint w,
                       const uint h,
                       const uint max_depth,
                       uint depth) const
    {
        // Compute range
        int yrange = y1 - y0;
        int xrange = x1 - x0;

        // Compute overall sum of this region
        float overall_sum = 0.0f;
        for (uint y=y0; y < y1; ++y)
            for (uint x=x0; x < x1; ++x)
                overall_sum += mono[x + y * w];

        float overall_half = overall_sum / 2.0f;

        // Check recursion condition
        if (depth < max_depth && yrange > 1 && xrange > 1)
        {
            // Looking for horizontal split
            if (yrange > xrange)
            {
                // Sum up pixel values until sum/2 is reached
                uint split_index = uint(-1);
                float sum = 0.0f;
                float last_sum = 0.0f;
                for (uint y=y0; y < y1; ++y)
                {
                    // Save last sum and add new row
                    last_sum = sum;
                    for (uint x=x0; x < x1; ++x)
                        sum += mono[x + y * w];

                    // Either taking the current y or the previous
                    // one. This depends on the absolute difference
                    // to the overall_sum
                    if (sum > overall_half)
                    {
                        split_index = sum-overall_half > overall_half-last_sum ?
                        y-1 : y;
                        break;
                    }
                }

                // Compute new regions
                /*for (uint x=x0; x < x1; ++x)
                {
                    modifiedPixels[x + (split_index-1) * w] = float4(1.0f,0.0f,0.0f,1.0f);
                    modifiedPixels[x + split_index * w] = float4(1.0f,0.0f,0.0f,1.0f);
                }*/
                computeRegion(lightSources, mono, pixels, x0, y0,
                              x1, split_index, w, h, max_depth, depth+1);
                computeRegion(lightSources, mono, pixels, x0, split_index,
                              x1, y1, w, h, max_depth, depth+1);
            }
            // Looking for vertical split
            else
            {
                // Sum up pixel values until sum/2 is reached
                uint split_index = uint(-1);
                float sum = 0.0f;
                float last_sum = 0.0f;
                for (uint x=x0; x < x1; ++x)
                {
                    // Save last sum and add new row
                    last_sum = sum;
                    for (uint y=y0; y < y1; ++y)
                        sum += mono[x + y * w];

                    // Either taking the current y or the previous
                    // one. This depends on the absolute difference
                    // to the overall_sum
                    if (sum > overall_half)
                    {
                        split_index = sum-overall_half > overall_half-last_sum ?
                        x-1 : x;
                        break;
                    }
                }

                // Compute new regions
                /*for (uint y=y0; y < y1; ++y)
                {
                    modifiedPixels[(split_index-1) + y * w] = float4(1.0f,0.0f,0.0f,1.0f);
                    modifiedPixels[split_index + y * w] = float4(1.0f,0.0f,0.0f,1.0f);
                }*/
                computeRegion(lightSources, mono, pixels, x0, y0,
                              split_index, y1, w, h, max_depth, depth+1);
                computeRegion(lightSources, mono, pixels, split_index, y0,
                             x1, y1, w, h, max_depth, depth+1);
             }
        }
        // Generate light for this region
        else
        {
            // Place light source at the energy centroid of this region
            // Compute overall sum of this region
            float sum = 0.0f;
            for (uint y=y0; y < y1; ++y)
            {
                for (uint x=x0; x < x1; ++x)
                {
                    sum += mono[x + y * w];
                    if (sum > overall_half)
                    {
                        //const float4 col = float4(0.0f,0.0f,1.0f,1.0f);
                        //const int off = 2;
                        //for (int i=max(y-off,0); i < min(y+off,(int)h); ++i)
                        //    for (int j=max(x-off,0); j < min(x+off,(int)w); ++j)
                        //        modifiedPixels[j + i * w] = col;

                        // Transform uv space to xyz space
                        printf("Light pixel: %d %d\n", x, y);
                        float u = y / (float)h;
                        float v = x / (float)w;
                        float twoPiRand = 2.0f * RTFACT_PI * v;
                        float temp = sqrtf(1.0f - u);
                        float px = cosf(twoPiRand) * temp;
                        float pz = sinf(twoPiRand) * temp;
                        float py = sqrtf(u);

                        // Sum up pixel values to compute light color
                        EnvironmentLight light;
                        light.direction = Vec3f1(-px, -py, -pz).normalize();
                        light.intensity = Vec3f1(0.0f, 0.0f, 0.0f);
                        for (uint j=y0; j < y1; ++j)
                            for (uint i=x0; i < x1; ++i)
                                light.intensity += pixels[i + j * w];
                        light.intensity /= (float)(xrange * yrange);
                        lightSources.push_back(light);
                        return;
                    }
                }
            }
        }
    }

public:

    template<
        class tBasicImage2D>
    void extractLights(const tBasicImage2D& aEnvMap,
                       const uint aPartitionCount,
                       Vector<EnvironmentLight>& aLightContainer) const
    {
        const uint w = aEnvMap.getResX();
        const uint h = aEnvMap.getResY();
        const uint size = w * h;
        const float temp = 1.0f / (h * RTFACT_INV_PI);

        Vec3f1 *data = new Vec3f1[size];
        float *lum = new float[size];
        for (uint y=0; y < h; ++y)
        {
            for (uint x=0; x < w; ++x)
            {
                const float *v = aEnvMap(x, y);
                const uint idx = x + y * w;

                data[idx] = Vec3f1(v[0], v[1], v[2]);
                lum[idx] = data[idx].luminance() * Math::sin(y * temp);
            }
        }

        // Compute lights
        computeRegion(aLightContainer, lum, data,
                      0, 0, w, h, w, h, aPartitionCount, 0);

        // Scale lights
        const float s = 1.0f / static_cast<float>(aLightContainer.size());
        for (uint i=0; i < aLightContainer.size(); ++i)
        {
            aLightContainer[i].intensity *= s;
        }
    }
};

RTFACT_NAMESPACE_END

#endif // RTFACT__ENVLIGHTGEN_HPP
