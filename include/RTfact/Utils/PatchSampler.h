#ifndef PATCHSAMPLER_H_
#define PATCHSAMPLER_H_
#define _USE_MATH_DEFINES
#include <math.h>
#include "Sampler.h"
#include "MersenneTwister.h"

template<typename tSample,
		 typename tHemisphereSample,
		 unsigned int tNumSets,
		 unsigned int tNumPatches,
		 unsigned int tSamplesPerPatch>
class PatchSampler
{
private:
	MTRand rnd;
	unsigned int count;
	unsigned int jump;
	unsigned int shuffledIndices[tNumSets * tNumPatches];
	tHemisphereSample patches[tNumSets * tNumPatches][tSamplesPerPatch];

	inline unsigned int computeRandomIndex()
	{
		if (count % tNumPatches == 0)
			jump = (rnd.randInt() % tNumSets) * tNumPatches;
		return jump + shuffledIndices[jump + count++ % tNumPatches];
	}
public:
	PatchSampler() : count(0u), jump(0u)
	{
		std::vector<unsigned int> indices;

		// Init indices vector
		for (unsigned int i=0; i < tNumPatches; ++i)
			indices.push_back(i);

		// Shuffle indices for each set
		unsigned int sidxPtr = 0u;
		for (unsigned int i=0; i < tNumSets; ++i)
		{
			random_shuffle(indices.begin(), indices.end());

			// Copy shuffed indices
			for (unsigned int j=0; j < tNumPatches; ++j)
				shuffledIndices[sidxPtr++] = indices[j];
		}
	}

	void generatePatches()
	{
		for (unsigned int s=0u; s < tNumSets; ++s)
		{
			// Generate numPatches samples and map them to a hemisphere
			Sampler<tSample, tHemisphereSample, tNumPatches * tSamplesPerPatch> sampler;
			sampler.generateMultiJittered();
			sampler.mapSamplesToHemisphere(1.0f);

			// Generate samplesPerPatch per patch and map them to a the
			// hemisphere with another cosine power distribution
			unsigned int n = (unsigned int)sqrtf(tNumPatches);
			unsigned int m = (unsigned int)sqrtf(tNumPatches * tSamplesPerPatch);
			unsigned int k = (unsigned int)sqrtf(tSamplesPerPatch);
			unsigned int patchPtr  = s * tNumPatches;
			unsigned int samplePtr = 0u;
			for (unsigned int i=0u; i < n; ++i)
			{
				for (unsigned int j=0u; j < n; ++j)
				{
					samplePtr = 0u;
					for (unsigned int p=0u; p < k; ++p)
					{
						for (unsigned int q=0u; q < k; ++q)
						{
							patches[patchPtr][samplePtr++] = sampler.hemisphereSamples[q + (p + i*k) * m + j * k];
						}
					}
					++patchPtr;
				}
			}
		}
	}

	std::vector<tHemisphereSample> sample()
	{
		tHemisphereSample *patch = patches[computeRandomIndex()];
		std::vector<tHemisphereSample> temp;
		for (unsigned int i=0; i < 4; ++i)
			temp.push_back(patch[i]);
		random_shuffle(temp.begin(), temp.end());
		return temp;
	}
};

#endif
