#include "RTfactShader.hpp"

SURFACE_SHADER
{
	result = Color(Context::getTime(context)/10000.f, 0, 0);

}
