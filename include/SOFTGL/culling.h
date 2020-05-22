// softgl culling.h
// Date: 05 04 2020
// Author: arinaivanova
// URL: https://github.com/arinaivanova
// Commentary: Hidden surface culling and invisible polygon cullling/clipping functions.

#ifndef SOFTGL_CULLING_H_
#define SOFTGL_CULLING_H_

#include "SOFTGL/gl.h"
#include "GMATH/vec2.h"

namespace gl
{
	//      /\ v0
	//     /  \      CCW: v0 -> v2 -> v1
	// v2 /    \ v1  CW:  v0 -> v1 -> v2
	// return true if triangle is considered front-facing relative to eye according to current orientation setting
	inline bool isFrontFace(uint8_t cullMode, uint8_t faceMode,
	                        const gmath::Vec2& v0, const gmath::Vec2& v1, const gmath::Vec2& v2)
	{
		if (!cullMode)
			return true;
		if (__builtin_popcount(cullMode) > 1)
			return false;
		if (gmath::edge(v0, v1, v2) < 0.f  ^ faceMode == FrontFace::CCW ^ cullMode & CullBit::back)
			return false;
		return true;
	}
	
	// Sutherland-Hodgman polygon culling and clipping. returns false if the polygon was culled.
	bool clipPolygon(AttribBuffer& out);
	
} // namespace gl

#endif
