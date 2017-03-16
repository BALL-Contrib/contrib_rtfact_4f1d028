#include "RTfactShader.hpp"

PARAMETER Color color;

SURFACE_SHADER
{
	result = normal + Color(.5f);
}
