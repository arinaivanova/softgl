// softgl sampler.h
// Date: 24 02 2020
// Author: arinaivanova
// URL: https://github.com/arinaivanova/softgl
// Commentary: Texture Sampler

#ifndef SOFTGL_SAMPLER_H_
#define SOFTGL_SAMPLER_H_

#include "GMATH/vec3.h"

namespace gl
{
	enum TextureType { diffuse, normal, spec };

	struct Sampler2D
	{
		unsigned char* diffuse {nullptr};
		unsigned char* normal  {nullptr};
		unsigned char* spec    {nullptr};
		int width;
		int height;
		int channels;

		gmath::Vec3 sample(const gmath::Vec2& texCoords, TextureType type) const;
	};
} // namespace softgl

#endif
