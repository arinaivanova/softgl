// softgl framebuffer.h
// Date: 09 02 2020
// Author: arinaivanova
// URL: https://github.com/arinaivanova/softgl
// Commentary: none provided.

#ifndef SOFTGL_FRAMEBUFFER_H_
#define SOFTGL_FRAMEBUFFER_H_

#include "GMATH/vec3.h"
#include <stdint.h>
#include <float.h>
#include <algorithm>

namespace gl
{
	enum BufBit : uint8_t
	{
		color = 1<<0,
		depth = 1<<1
	};

	class FrameBuffer
	{
	public:
		gmath::Vec3* colorBuf;
		float* depthBuf;
		unsigned int width;
		unsigned int height;
		float aspect;

		FrameBuffer(unsigned int _width, unsigned int _height)
		  :  width{_width}, height{_height}, aspect{_width/float(_height)}
		{
			colorBuf = new gmath::Vec3[width*height];
			depthBuf = new float[width*height];
		}

		void clear(uint8_t bufs)
		{
			if (bufs & BufBit::color)
				std::fill(colorBuf, colorBuf+width*height, gmath::Vec3{});
			if (bufs & BufBit::depth)
				std::fill(depthBuf, depthBuf+width*height, FLT_MAX);
		}

		~FrameBuffer()
		{
			delete[] colorBuf;
			colorBuf = nullptr;
			delete[] depthBuf;
			depthBuf = nullptr;
		}
	};
	
} // namespace gl

#endif
