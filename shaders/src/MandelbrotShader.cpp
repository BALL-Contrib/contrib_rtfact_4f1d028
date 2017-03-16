#include "RTfactShader.hpp"

PARAMETER float min_cx;
PARAMETER float min_cy;
PARAMETER float pa_x;
PARAMETER float pa_y;
PARAMETER float translateX;
PARAMETER float translateY;
PARAMETER float scale;
PARAMETER int maxIterations;
PARAMETER float maxSquaredValue;
PARAMETER int colored;

BEGIN_INIT_PARAM
INIT_PARAM(min_cx, -2.0f)
INIT_PARAM(min_cy, -1.0f)
INIT_PARAM(pa_x, 3.0f)
INIT_PARAM(pa_y, 2.25f)
INIT_PARAM(translateX, 0.0f)
INIT_PARAM(translateY, 0.0f)
INIT_PARAM(scale, 1.0f)
INIT_PARAM(maxIterations, 80)
INIT_PARAM(maxSquaredValue, 100000.0f)
INIT_PARAM(colored, 1)
END_INIT_PARAM

SURFACE_SHADER
{
	Color color;

	//for(int i = 0; i < 20; ++i)
	{
		float x = scale * texCoord1 + translateX;
		float y = scale * texCoord2 + translateY;

		float cx = min_cx + x * pa_x;
		float cy = min_cy + y * pa_y;

		int iter = 0;
		float squaredAbs = 0.f;

		float x2 = 0.f;
		float y2 = 0.f;

		//value
		while ((squaredAbs <= maxSquaredValue) && (iter <
		maxIterations)) {
			 float xt = x2 * x2 - y2 * y2 + cx;
			 float yt = 2.f * x2 * y2 + cy;
			 x2 = xt;
			 y2 = yt;
			 iter++;
			 squaredAbs = x2 * x2 + y2 * y2;
		}

		if (colored) {
			//color
			float itkont = iter - logf(logf(squaredAbs) / logf(4.f)) / logf(2.f);
			float d = itkont / (float) maxIterations;

			if (d < 0.2f) color = Color(5.f * d,5.f * d,1.0f);
			if (d >= 0.2f && d < 0.4f) color = Color(1.0f, 1.0f,1.0f-(5.f*(d-0.2f)));
			if (d >= 0.4f && d < 0.6f) color = Color(1.0f, 1.0f - (5.f*(d-0.4f)),0.0f);
			if (d >= 0.6f && d < 0.8f) color = Color(1.0f- (5.f*(d-0.6f)),0.0f,5.f*(d-0.6f));
			if (d >= 0.8f && d < 1.0f) color = Color(5.f*(d-0.8f),5.f*(d-0.8f),1.0f);

			//divergiert nicht nach gegebenen max_iterationen
			//=> bq < max_betrags_quadrat, deshalb max_iter erreicht
			if (iter == maxIterations) color = Color(0.f);
		} else {
			if (iter != maxIterations) {
				color = Color(1.0f,1.0f,1.0f);
			}
		}
	}
	result = Max(Min(color,Color(1.0f)), Color(0.f));
}
