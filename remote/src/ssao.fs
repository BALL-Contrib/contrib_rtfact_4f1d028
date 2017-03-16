#extension GL_ARB_texture_rectangle : enable

/*
_is = in image space
_es = in eye space
*/

uniform sampler2DRect uTexture;
uniform int uResX;
uniform int uResY;
uniform sampler2DRect uDistanceTexture;
uniform float uAORadius;
uniform float uFocusDistance;
uniform float uDistanceScale;
uniform sampler2DRect uNormalTexture;
uniform mat3 uTransform_ws2es;

const int DIRECTION_COUNT=8;
const float INV_DIRECTION_COUNT=0.125;
const float DIRECTION_STEP=0.785398;    //PI/4

const float MAX_SAMPLE_COUNT=8.0;
const float SAMPLE_STEP=2.0;

const float EPS=1e-2;

const float uTangentBias=-0.523599;  //PI/6

vec3 cameraX=0.0.xxx,cameraY=0.0.xxx,cameraZ=0.0.xxx;
vec2 screenHalf=vec2(0.5*float(uResX),0.5*float(uResY));



/*
gl_TexCoord[0].xy point at pixel center (that is, x.5;y.5), with (0,0) being the bottom left-hand corner and (uResX-1;uResY-1) the top right-hand corner
*/

/*
Coordinates of the image plane rectangle in eye space are:
    ( -aspectRatio, -1, focusDistance ) x ( aspectRatio, 1, focusDistance)
*/



vec4 gammaCorrection(vec4 aColor,float aGamma)
{
    float gammaRecip = 1.0 / aGamma;
    return pow(aColor,
               vec4(gammaRecip, gammaRecip, gammaRecip, 1.0));
}
//--------------------------------------------------------------------------------------------------
void computeCameraVectors()
{
    float invResY=1.0/float(uResY);
    cameraX=vec3(2.0*invResY, 0.0, 0.0);
    cameraY=vec3(0.0, 2.0*invResY, 0.0);
    cameraZ=vec3(-float(uResX)*invResY, -1.0, uFocusDistance);
}
//--------------------------------------------------------------------------------------------------
vec3 transform_ws2es(
    vec3 aVector_ws)
{
    return uTransform_ws2es*aVector_ws;
}
//--------------------------------------------------------------------------------------------------
vec3 getPixelNormal(
    vec2 aPixel_is)
{
    return -1.0+2.0*texture2DRect(uNormalTexture,aPixel_is).xyz;
}
//--------------------------------------------------------------------------------------------------
vec2 project(
    vec3 aPoint_es)
{
    float scale=uFocusDistance/aPoint_es.z;
    return 0.5*float(uResY)*scale*aPoint_es.xy+screenHalf;
}
//--------------------------------------------------------------------------------------------------
vec3 castRay(
    vec2 aSample_is)
{
    return normalize(
        cameraX*aSample_is.x+
        cameraY*aSample_is.y+
        cameraZ);
}
//--------------------------------------------------------------------------------------------------
vec3 unproject(
    vec3 aVector_is)
{
    //ToDo: make it work correctly (there seems to be slight error in Z - it's slightly spherical)
    vec3 ray=castRay(aVector_is.xy);
    return ray*aVector_is.z*uDistanceScale;
}
//--------------------------------------------------------------------------------------------------
float projectCircleRadius(
    float aRadius,
    vec3 aPoint_es,
    vec2 aPoint_is)
{
    vec2 projected=project(aPoint_es+vec3(aRadius,0.0,0.0));
    return (projected-aPoint_is).x;
}
//--------------------------------------------------------------------------------------------------
vec2 snapToPixel(
    vec2 aPoint)
{
    return floor(aPoint)+0.5.xx;
}
//--------------------------------------------------------------------------------------------------
vec3 samplePlane(
    vec2 aSample,
    vec4 aPlane)
{
    vec3 ray=castRay(aSample);
    return ray * -aPlane.w/dot(ray,aPlane.xyz);
}
//--------------------------------------------------------------------------------------------------
vec3 differential(
    vec3 aLesser,
    vec3 aCenter,
    vec3 aGreater)
{
    return 0.5*(aGreater-aLesser);
}
//--------------------------------------------------------------------------------------------------
float attenuateAO(
    float aQuotient2)
{
    return 1.0-aQuotient2;
}
//--------------------------------------------------------------------------------------------------
//ToDo: once debugged, change return type to float
vec3 computeAO(
    vec3 aCurrentFragment_is)
{
    if (
        aCurrentFragment_is.z==1.0)
    {
        //no AO on background fragments
        return 0.0.xxx;
    }
    float dirAngle=0.0;
    vec3 currentFragment_es=unproject(aCurrentFragment_is);
    vec3 normal_es=transform_ws2es(getPixelNormal(aCurrentFragment_is.xy));
    float aoRadius_es = ( uAORadius!=0.0 ? uAORadius : 0.5 );  //temp
    float aoRadius_is=projectCircleRadius(aoRadius_es,currentFragment_es,aCurrentFragment_is.xy);
    float aoRadius2_es=aoRadius_es*aoRadius_es;
    float invAORadius2_es=1.0/aoRadius2_es;
    float totalAO=0.0;
    vec3 dFes_dXis,dFes_dYis;   //d(Fragment_es)/d(X_is), d(Fragment_es)/d(Y_is)
    {
        vec4 plane=vec4(normal_es,-dot(normal_es,currentFragment_es));
        vec3 east_es=samplePlane(aCurrentFragment_is.xy+vec2(1.0,0.0),plane);
        vec3 west_es=samplePlane(aCurrentFragment_is.xy+vec2(-1.0,0.0),plane);
        vec3 north_es=samplePlane(aCurrentFragment_is.xy+vec2(0.0,1.0),plane);
        vec3 south_es=samplePlane(aCurrentFragment_is.xy+vec2(0.0,-1.0),plane);
        dFes_dXis=differential(west_es,currentFragment_es,east_es);
        dFes_dYis=differential(south_es,currentFragment_es,north_es);
    }
    for (int idxDir=0;idxDir<DIRECTION_COUNT;++idxDir,dirAngle+=DIRECTION_STEP)
    {
        float sinHorizonAngle=0.0;
        vec2 currentSample_is=aCurrentFragment_is.xy;
        vec2 dir=vec2(cos(dirAngle),sin(dirAngle));
        vec2 dirStep=dir*(SAMPLE_STEP/max(abs(dir.x),abs(dir.y)));  //dir scaled so that the abs-greater component is +-SAMPLE_STEP
        float sinTangent;
        {
            vec3 tangent_es=(dirStep.x*dFes_dXis + dirStep.y*dFes_dYis);
            float tangentAngle=max( uTangentBias+atan(tangent_es.z,length(tangent_es.xy)), -1.57096 /*PI/2*/ );
            sinTangent=sin(tangentAngle);
        }
        float lastAO=0.0;
        int sampleCount=int(min(MAX_SAMPLE_COUNT,ceil(aoRadius_is)));
        for (
            int idxSample=0;idxSample<sampleCount;++idxSample)
        {
            currentSample_is+=dirStep;
            vec2 currentSamplePixel_is=snapToPixel(currentSample_is);
            if (
                any(lessThan(currentSamplePixel_is,vec2(0.0,0.0))) ||
                any(greaterThan(currentSamplePixel_is,vec2(float(uResX),float(uResY)))))
            {
                //moved out of image - terminate this direction
                break;
            }
            vec3 currentSample_es=unproject(vec3(currentSamplePixel_is, texture2DRect(uDistanceTexture,currentSamplePixel_is)));
            float dist2_es=dot(currentSample_es-currentFragment_es,currentSample_es-currentFragment_es);
            if (
                dist2_es<=aoRadius2_es)
            {
                float sampleSinHorizonAngle=(currentSample_es.z-currentFragment_es.z)/sqrt(dist2_es);
                if (
                    sinHorizonAngle>sampleSinHorizonAngle+EPS)  //remember the angles are negative
                {
                    sinHorizonAngle=sampleSinHorizonAngle;
                    float ao=max(sinTangent-sinHorizonAngle,0.0);
                    totalAO+=attenuateAO(dist2_es*invAORadius2_es)*(ao-lastAO);
                    lastAO=ao;
                }
            }
        }
    }
    return (totalAO*INV_DIRECTION_COUNT).xxx;
}
//--------------------------------------------------------------------------------------------------
void main()
{
    computeCameraVectors();
    vec4 color = texture2DRect(uTexture, gl_TexCoord[0].xy);
    vec3 ambientOcclusion = computeAO(vec3(gl_TexCoord[0].xy, texture2DRect(uDistanceTexture,gl_TexCoord[0].xy)));
    if (
        true)
    {
        color.rgb *= 1.0-ambientOcclusion;
        gl_FragColor = gammaCorrection(color, 1.4);
    }
    else
    {
        //branch for debugging purposes only
        color.rg = ambientOcclusion.rg;
        gl_FragColor = color;
    }
}
