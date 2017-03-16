#ifndef RTFACT__IESLIGHT_HPP
#define RTFACT__IESLIGHT_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Concept/Light.hpp>
#include <RTfact/Model/LightAttenuation/OpenGLLightAttenuation.hpp>
#include <RTfact/Model/Light/Detail/IESUtils.hpp>
#include <RTfact/Utils/Matrix4f.hpp>

RTFACT_NAMESPACE_BEGIN

template<
    class tLightAttenuation>
class IESLight :
    public Light
{
    enum Symmetry
    {
        SYMMETRY_90,
        SYMMETRY_180,
        SYMMETRY_FULL,
        SYMMETRY_NONE
    };

    RTFACT_LIGHT_INLINE float getIESScale(
        const Vec3f1& aDirectionToLight) const
    {
        RTFACT_LIGHT_ASSERT(mInitialized);
        if(!mInitialized)
          return 0.0f;

        float c_phi = Math::abs(Math::degrees(
            Math::atan2(aDirectionToLight.y, -aDirectionToLight.x)));

        float g_theta =
            Math::min(Math::degrees(Math::acos(aDirectionToLight.z)), mMaxVertAngle);

        const int numHoriz = mResHoriz - 1;
        const int numVert = mResVert - 1;

        const float x = (c_phi / mMaxHorizAngle) * numHoriz;
        const float y = (g_theta / mMaxVertAngle) * numVert;

        RTFACT_ASSERT(x <= numHoriz);
        RTFACT_ASSERT(y <= numVert);

        const float ixf = Math::floor(x);
        const float iyf = Math::floor(y);

        const float fracX = x - ixf;
        const float fracY = y - iyf;

        const int ix = static_cast<int>(ixf);
        const int iy = static_cast<int>(iyf);

        const int ix1 = (ix < numHoriz) ? ix + 1 : 0;
        const int iy1 = (iy < numVert)  ? iy + 1 : 0;

        const int yIndex = iy * mResHoriz;
        const int y1Index = iy1 * mResHoriz;

        return Math::interpolate(
            fracX, fracY,
            mData[yIndex + ix],  mData[yIndex + ix1],
            mData[y1Index + ix],  mData[y1Index + ix1]);
    }

    void recomputeLocalCoord()
    {
        Vec3f1 normalizedDirection = mDirection.normalize();
        Vec3f1 normalizedBaseVector = mBaseVector.normalize();
        Vec3f1 upvector = Vec3f1::cross(normalizedDirection, normalizedBaseVector);
        Vec3f1 base = Vec3f1::cross(upvector, normalizedDirection);

        mLocalCoord = Matrix4f (
            base.x, upvector.x, -normalizedDirection.x, 0,
            base.y, upvector.y, -normalizedDirection.y, 0,
            base.z, upvector.z, -normalizedDirection.z, 0,
            0, 0, 0, 1);

        Matrix4f offsetRotation(cos(mOffsetAngle), sin(mOffsetAngle), 0, 0,
                                -sin(mOffsetAngle), cos(mOffsetAngle), 0,  0,
                                0, 0, 1, 0,
                                0, 0, 0, 1);

        Matrix4f zRotatedLocalCoord = mLocalCoord*offsetRotation;
        base = zRotatedLocalCoord.multiplyVectorAffine(Vec3f1(1,0,0));
        upvector = zRotatedLocalCoord.multiplyVectorAffine(Vec3f1(0,1,0));

        mLocalCoord = Matrix4f (
          base.x, base.y, base.z, 0,
          upvector.x, upvector.y, upvector.z, 0,
          -normalizedDirection.x, -normalizedDirection.y, -normalizedDirection.z, 0,
          0, 0, 0, 1);
    }

    void cleanUp()
    {
        if(mData != NULL)
        {
            delete [] mData;
            mData = NULL;
            mInitialized = false;
        }
    }

    bool mInitialized;
    float mMaxVertAngle, mMaxHorizAngle, mAvgIESScale;
    std::string mFile;
    float* mData;
    uint mResHoriz, mResVert;

    Vec3f1 mPosition;
    Vec3f1 mDirection;
    Vec3f1 mBaseVector;
    float mOffsetAngle;
    float mIntensity;
    Vec3f1 mColor;
    tLightAttenuation mAttenuation;
    Matrix4f mLocalCoord;

public:

    struct IESDefinitionData
    {
        typedef RTfact::Vector<float>   tDataVector;

        tDataVector dataVector;
        float       maximumVerticalAngle;
        float       maximumHorizontalAngle;
        float       averageIESScale;
        uint        horizontalResolution;
        uint        verticalResolution;
    };

    //PROPERTIES ------------------------------------------------------------------
    RTFACT_DEFINE_PROPERTY(Vec3f1&,             Position);
    RTFACT_DEFINE_PROPERTY(Vec3f1&,             Color);
    RTFACT_DEFINE_PROPERTY(float,               Intensity);
    RTFACT_DEFINE_PROPERTY(tLightAttenuation&,  Attenuation);

    void getIESDefinitionData(IESDefinitionData &aData)
    {
        for(uint i = 0; i < mResVert * mResHoriz; i++)
        {
            aData.dataVector.push_back(mData[i]);
        }
        aData.maximumHorizontalAngle = mMaxHorizAngle;
        aData.maximumVerticalAngle = mMaxVertAngle;
        aData.averageIESScale = mAvgIESScale;
        aData.horizontalResolution = mResHoriz;
        aData.verticalResolution = mResVert;
    }

    void setIESDefinitionData(const IESDefinitionData &aData)
    {
        const uint bufferSize = aData.horizontalResolution * aData.verticalResolution;
        if(aData.dataVector.size() < bufferSize)
        {
            RTFACT_LOG_ERROR_F("IESDefinitionData.dataVector contains less \
                items than indicated by horizontal and vertical resolutions \
                (%1%x%2% vs. %3%)", aData.horizontalResolution % aData.verticalResolution % aData.dataVector.size());
            return;
        }

        cleanUp();

        mResHoriz = aData.horizontalResolution;
        mResVert = aData.verticalResolution;
        mAvgIESScale = aData.averageIESScale;
        mMaxVertAngle = aData.maximumVerticalAngle;
        mMaxHorizAngle = aData.maximumHorizontalAngle;

        mData = new float[int(mMaxHorizAngle * mMaxVertAngle)];
        for(uint i = 0; i < int(mMaxHorizAngle * mMaxVertAngle); i++)
        {
            mData[i] = aData.dataVector[i];
        }
        mInitialized = true;
    }

    const bool isInitialized() const { return mInitialized; }

    const float getOffsetAngle() const { return mOffsetAngle; }
    void setOffsetAngle(const float aOffsetAngle) {
        mOffsetAngle = aOffsetAngle;
        recomputeLocalCoord();
    }

    const Vec3f1& getDirection() const { return mDirection; }
    void setDirection(const Vec3f1& aDirection) {
        mDirection = aDirection;
        recomputeLocalCoord();
    }

    const Vec3f1& getBaseVector() const { return mBaseVector; }
    void setBaseVector(const Vec3f1& aBaseVector) {
        mBaseVector = aBaseVector;
        recomputeLocalCoord();
    }

    std::string getFile() { return mFile; }
    bool setFile(const std::string& aFile)
    {
        mFile = aFile;

        cleanUp();

        Detail::IE_DATA inputData;
        if(!Detail::IE_ReadFile(mFile.c_str(), &inputData))
        {
            RTFACT_LOG_ERROR_F("IES: Error reading file %1%", mFile);
            return false;
        }

        mResVert = inputData.photo.num_vert_angles;
        const uint iesResHoriz = static_cast<uint>(inputData.photo.num_horz_angles);

        mMaxVertAngle = inputData.photo.vert_angles[mResVert - 1];
        mMaxHorizAngle = Math::max(inputData.photo.horz_angles[iesResHoriz - 1], 1.f);

        Symmetry symmetry;

        if(iesResHoriz == 1)            // full symmetry around Z axis
        {
            symmetry = SYMMETRY_FULL;
            mResHoriz = 2;
            mMaxHorizAngle = RTFACT_INFINITY;
        }
        else if(mMaxHorizAngle == 90.f)  // 4-quadrant symmetry
        {
            symmetry = SYMMETRY_90;
            mResHoriz = iesResHoriz * 4;
            mMaxHorizAngle = 360.f;
        }
        else if(mMaxHorizAngle == 180.f) // hemicircle symmetry
        {
            symmetry = SYMMETRY_180;
            mResHoriz = iesResHoriz * 2;
            mMaxHorizAngle = 360.f;
        }
        else if(mMaxHorizAngle)          // no symmetry
        {
            symmetry = SYMMETRY_NONE;
            mResHoriz = iesResHoriz;
        }

        mData = new float[mResVert * mResHoriz];
        mAvgIESScale = 0.f;

        for(uint v = 0; v < mResVert; ++v)
        {
            for(uint h = 0; h < iesResHoriz; ++h)
            {
                const uint baseIndex = v * mResHoriz + h;

                mData[baseIndex] = inputData.photo.pcandela[h][v];
                mAvgIESScale += mData[baseIndex];

                switch(symmetry)
                {
                case SYMMETRY_NONE:
                    break;
                case SYMMETRY_90:
                    mData[baseIndex + 2 * iesResHoriz] = inputData.photo.pcandela[h][v];
                    mData[baseIndex + 3 * iesResHoriz] = inputData.photo.pcandela[iesResHoriz - 1 - h][v];
                default:
                    mData[baseIndex + iesResHoriz] = inputData.photo.pcandela[iesResHoriz - 1 - h][v];
                }
            }
        }

        mAvgIESScale /= (mResHoriz * mResVert);

        mInitialized = true;
        return true;
    }

    //CONSTRUCTORS ------------------------------------------------------------------
    RTFACT_LIGHT_INLINE IESLight() :
        Light(Light::HAS_COMMON_SAMPLE_POS |
              Light::IS_DELTA |
              Light::CASTS_SHADOWS),
        mInitialized(false),
        mAvgIESScale(0.f),
        mPosition(0.f, 0.f, 0.f),
        mDirection(0.f, 0.f, 1.f),
        mBaseVector(1.f, 0.f, 0.f),
        mOffsetAngle(0.f),
        mColor(1.f, 1.f, 1.f),
        mIntensity(1.f),
        mAttenuation(0.f, 0.f, 1.f),
        mLocalCoord(1, 0, 0, 0, //base
                    0, 1, 0, 0, //up
                    0, 0,-1, 0, //direction
                    0, 0, 0, 1),
        mData(NULL)
    {}

    RTFACT_LIGHT_INLINE ~IESLight()
    {
        cleanUp();
    }

    //INTERFACE ------------------------------------------------------------------
    template<
        uint taSize>
    RTFACT_LIGHT_INLINE void illuminatePoint(
        const Vec2f<taSize>& aPositionSamples,
        const Vec3f<taSize>& aPoint,
        const ActiveMask<taSize>& aMask,
        Vec3f<taSize>& oDirection,
        Packet<taSize, float>& oDistance,
        Vec3f<taSize>& oRadiance,
        Vec3f<1> *oCommonSamplePosition) const
    {
        typedef Packet<taSize, float>        t_Packet;
        typedef typename t_Packet::Container t_PacketC;
        typedef Vec3f<taSize>                t_Vec3f;
        typedef typename t_Vec3f::Container  t_Vec3fC;

        RTFACT_ASSERT(oCommonSamplePosition != NULL);
        *oCommonSamplePosition = mPosition;

        const t_Vec3fC colorC = t_Vec3fC::replicate(mColor.normalize());

        RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aMask)
        {
            oDirection(i) = mPosition - aPoint(i);
            oDistance(i) = oDirection(i).length();
            oDirection(i) *= Math::reciprocal(oDistance(i));

            RTFACT_PACKET_FOR_ALL(j, t_Vec3fC::SIZE)
            {
                if(aMask(i)[j])
                {
                    Vec3f1 directionToLight = mLocalCoord.multiplyVectorAffine(oDirection(i)[j]);

                    const float iesScale = getIESScale(directionToLight);
                    oRadiance(i).x[j] = iesScale * mIntensity;
                    oRadiance(i).y[j] = iesScale * mIntensity;
                    oRadiance(i).z[j] = iesScale * mIntensity;
                }
            }
            oRadiance(i) *= colorC * mAttenuation.getFactor(oDistance(i));
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
        oPosition = Vec3f<taSize>::replicate(mPosition);

        typedef Packet<taSize, float>        t_Packet;
        typedef typename t_Packet::Container t_PacketC;
        typedef Vec3f<taSize>                t_Vec3f;
        typedef typename t_Vec3f::Container  t_Vec3fC;
        typedef Vec2f<taSize>                t_Vec2f;
        typedef typename t_Vec2f::Container  t_Vec2fC;

        const t_Vec3fC colorC = t_Vec3fC::replicate(mColor.normalize());

        RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aMask)
        {
            t_Vec2fC uvSamples = aPositionSamples(i);//(aU(i), aV(i));
            t_PacketC pdf;
            t_Vec3fC direction;

            direction = Math::uniformSphere(uvSamples, &pdf);
            oDirection(i) = direction;
            oPdf(i) = pdf;

            RTFACT_PACKET_FOR_ALL(j, t_Vec3fC::SIZE)
            {
                if(aMask(i)[j])
                {
                    Vec3f1 localDirection =
                        mLocalCoord.multiplyVectorAffine(direction[j]);
                    const float iesScale = getIESScale(localDirection);
                    oRadiance(i).x[j] = iesScale * mIntensity;
                    oRadiance(i).y[j] = iesScale * mIntensity;
                    oRadiance(i).z[j] = iesScale * mIntensity;
                }
            }

            oRadiance(i) *= colorC;
        }
    }

    RTFACT_LIGHT_INLINE const Vec3f1 getTotalPower() const
    {
        RTFACT_LIGHT_ASSERT(mInitialized);

        return Vec3f1(RTFACT_4_PI * mAvgIESScale, RTFACT_4_PI * mAvgIESScale, RTFACT_4_PI * mAvgIESScale);
    }

    #include RTFACT_LIGHT_VIRTUAL_METHODS
};

RTFACT_NAMESPACE_END

#endif // RTFACT__IESLIGHT_HPP
