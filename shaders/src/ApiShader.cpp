#include "RTfactShader.hpp"

SURFACE_SHADER
{
	Point x = normal + u*v;
		float c = cosf(hitDistance * .1f); //replace floor by cos
		int b = 0;

		for (int i=0; i<1000; ++i) {
			c += expf(hitDistance * -0.04f+i); //replace floor by exp
			if (c > 100.f) {
				c *= u-v / Dot(origin - dir, normal);
				result = Color(c);
				b = 1; //return;
				break;
			}
			c *= logf(Dot(vertColor, vertColor)); //replace floor by log/exp/...
		}

	    if (b != 0) return;

		result = (x - normal) * c;
		Normalize(result);
}
