// softgl culling.cpp
// Date: 05 04 2020
// Author: arinaivanova
// URL: https://github.com/arinaivanova/softgl
// Commentary: for homogeneous polygon culling/clipping reference, see
//             https://www.microsoft.com/en-us/research/wp-content/uploads/1978/01/p245-blinn.pdf

#include "SOFTGL/culling.h"

using namespace gl;
using namespace gmath;

// Sutherland-Hodgman polygon clipper
bool gl::clipPolygon(AttribBuffer& out)
{
	static const gmath::Vec4 frustumPlanes[6]
		{
			{ 0.f,  0.f, -1.f, 1.f}, // near
			{ 0.f,  0.f,  1.f, 1.f}, // far
			{ 0.f, -1.f,  0.f, 1.f}, // right
			{ 0.f,  1.f,  0.f, 1.f}, // left
			{-1.f,  0.f,  0.f, 1.f}, // top
			{ 1.f,  0.f,  0.f, 1.f}  // bottom
		};
	AttribBuffer in=out;
	out.clear();

	for (auto& plane : frustumPlanes)
	{
		for (int i=0; i<in.size(); ++i)
		{
			// vertices of current edge
			const Attrib& v0 = in[i];
			const Attrib& v1 = in[i+1==in.size()? 0 : i+1];

			// compute each distance from plane to vertex
			const float t0 = dot(plane, v0.vertex);
			const float t1 = dot(plane, v1.vertex);

			// vertices on the positive side of plane (determined by plane normal) are "inside"
			const bool inside0 = t0>=0.f;
			const bool inside1 = t1>=0.f;
			
			// first vertex is inside: keep first vertex
			if (inside0)
			{
				out.push_back(v0);
				// first vertex is inside, second is outside: keep intersection
				if (!inside1)
				{
					// linearly interpolate line intersection 
					const float t = t1/(t1-t0);
					out.push_back(Attrib{ lerp(v1.vertex, v0.vertex, t),
					                      v0.normal,
					                      lerp(v1.texCoord, v0.texCoord, t) });
				}
			}
			// first vertex is outside, second vertex is inside: keep intersection
			else if (inside1)
			{
				const float t = t1/(t1-t0);
				out.push_back(Attrib{ lerp(v1.vertex, v0.vertex, t),
				                      v0.normal,
				                      lerp(v1.texCoord, v0.texCoord, t) });
			}
		}
		// discard polygon outside of frustum
		if (out.size()<3)
			return false;
		in=out;
		out.clear();
	}
	// form triangles from clipped vertex pool
	for (int i=1; i<in.size()-1; ++i)
	{
		out.push_back(in[0]);
		out.push_back(in[i]);
		out.push_back(in[i+1]);
	}
	return true;
}
