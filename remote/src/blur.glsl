#extension GL_ARB_texture_rectangle : enable



uniform float uBlurFalloff;
uniform float uBlurSharpness;
uniform sampler2DRect uDistanceTexture;



float blurKernel(
    float aSampleAO,
    float aPixelDiff,
    float aDistDiff,
    out float aAccumulatedWeight)
{
    float weight = exp(-aPixelDiff*aPixelDiff*uBlurFalloff - aDistDiff*aDistDiff*uBlurSharpness);
    aAccumulatedWeight += weight;
    return weight*aSampleAO;
}
//--------------------------------------------------------------------------------------------------
float blur(
    vec2 aAxis,
    vec3 aCurrentFragment)
{
    float accumulatedAO=0.0;
    float accumulatedWeight=0.0;
    for (
        float sampleDist=-uKernelRadius;
        sampleDist<=uKernelRadius;
        ++sampleDist)
    {
        vec2 sample=aCurrentFragment.xy+sampleDist*aAxis;
        accumulatedAO+=blurKernel(
            texture2DRect(uAOTexture,sample),
            sampleDist,
            aCurrentFragment.z-texture2DRect(uDistanceTexture,sample),
            accumulatedWeight);
    }
    return accumulatedAO/accumulatedWeight; //accumulatedWeight is guaranteed !=0, because currentFragment's weight is always 1
}
