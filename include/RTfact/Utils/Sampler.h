#ifndef SAMPLER_H_
#define SAMPLER_H_
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <algorithm>
#include <RTfact/Utils/Math/MathUtils.hpp>
#include <RTfact/Utils/Math/SamplingUtils.hpp>

template<typename tSample,
		 typename tHemisphereSample,
		 unsigned int tNumSamples>
class Sampler
{
private:
	unsigned int numSamples;
	unsigned int count;
	unsigned int shuffledIndices[tNumSamples];
	tSample samples[tNumSamples];
	tHemisphereSample hemisphereSamples[tNumSamples];

    inline float randFloat(float l, float h)
    {
        return RTfact::Math::uniformFloat() * (h - l) + l;
    }

    inline int randInt(int l, int h)
    {
        return (int) (randFloat(0.f, h - l + 1.f) + l);
    }

    void shuffleCoordinatesX(const unsigned int num)
    {
        for (unsigned int i = 0; i < num; i++)
        {
            for (unsigned int j = 0; j < num; j++)
            {
                int k = randInt(j, num - 1);
                float t = samples[i * num + j].x;
                samples[i * num + j].x = samples[i * num + k].x;
                samples[i * num + k].x = t;
            }
        }
    }

    void  shuffleCoordinatesY(const unsigned int num)
    {
        for (unsigned int i = 0; i < num; i++)
        {
            for (unsigned int j = 0; j < num; j++)
            {
                int k = randInt(j, num - 1);
                float t = samples[j * num + i].y;
                samples[j * num + i].y = samples[k * num + i].y;
                samples[k * num + i].y = t;
            }
        }
    }

public:
	Sampler() : numSamples(tNumSamples),
				count(0u)
	{
		// Set up indices
		std::vector<unsigned int> indices;
		for (unsigned int i=0; i < tNumSamples; ++i)
			indices.push_back(i);

		// Shuffle indices
		random_shuffle(indices.begin(), indices.end());

		// Copy back
		memcpy(shuffledIndices, &indices[0], tNumSamples * sizeof(unsigned int));

        // Seed random generator
        RTfact::Math::seedRandom();
	}

	void generateMultiJittered()
	{
		unsigned int num = (unsigned int)sqrtf((float)tNumSamples);
		float subcell_width = 1.0f / (float)tNumSamples;
		for (unsigned int i=0; i < num; ++i)
			for (unsigned int j=0; j < num; ++j)
				samples[j + i * num] = tSample((i * num + j) * subcell_width + randFloat(0.0f, subcell_width),
											   (j * num + i) * subcell_width + randFloat(0.0f, subcell_width));

		// Shuffle coordinates
		shuffleCoordinatesX(num);
		shuffleCoordinatesY(num);
	}

	void mapSamplesToHemisphere(const float e)
	{
		for (unsigned int i=0; i < tNumSamples; ++i)
		{
			float cosPhi = cosf(2.0f * (float)M_PI * samples[i].x);
			float sinPhi = sinf(2.0f * (float)M_PI * samples[i].x);
			float cosTheta = powf((1.0f - samples[i].y), 1.0f / (e + 1.0f));
			float sinTheta = sqrtf(1.0f - cosTheta * cosTheta);
			float pu = sinTheta * cosPhi;
			float pv = sinTheta * sinPhi;
			float pw = cosTheta;
			hemisphereSamples[i] = tHemisphereSample(pu, pv, pw);
		}
	}

	tHemisphereSample sampleHemisphere()
	{
		return hemisphereSamples[shuffledIndices[count++ % tNumSamples]];
	}

    tHemisphereSample getSample(const unsigned int idx)
    {
        return hemisphereSamples[idx % tNumSamples];
    }
};

#endif
