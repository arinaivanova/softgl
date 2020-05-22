// softgl sampler.cpp
// Date: 24 02 2020
// Author: arinaivanova
// URL: https://github.com/arinaivanova/softgl
// Commentary: Texture Sampler

#include "SOFTGL/sampler.h"
#include <cmath>

using namespace gl;

gmath::Vec3 Sampler2D::sample(const gmath::Vec2& texCoords, TextureType type) const
{
	const int s = floor(width * texCoords.u);
	const int t = floor(height * (1.f - texCoords.v));
	uint idx = (t * width + s) * channels;
	switch (type)
	{
	case TextureType::diffuse:
		return gmath::Vec3{diffuse[idx]/255.f, diffuse[++idx]/255.f, diffuse[++idx]/255.f};
		break;
	case TextureType::normal:
		return gmath::normalize(gmath::Vec3{normal[idx]/255.f, normal[++idx]/255.f, normal[++idx]/255.f}*2.f-1.f);
		break;
	case TextureType::spec:
		return gmath::Vec3{(float)spec[idx], (float)spec[++idx], (float)spec[++idx]};
		break;
	default:
		return gmath::Vec3{};
	}
}
