// softgl gl.cpp
// Date: 04 04 2020
// Author: arinaivanova
// URL: https://github.com/arinaivanova/softgl
// Commentary: none provided.

#include <assert.h>
#include "SOFTGL/gl.h"
#include "SOFTGL/culling.h"
#include "GMATH/util.h"

using namespace gl;
using namespace gmath;

void gl::drawIndexedArrays(Context* ctx, const IndexBuffer& idxBuf)
{
	// debugging
	assert(idxBuf.size() >= ctx->primMode * 3);
	
	const Uniform& uniform = *ctx->uniform;
	const AttribArrays& attribArr = *ctx->attribArr;
	
	AttribBuffer attribBuf;
	// vertex processing
	for (int i=0; i<idxBuf.size()-2;)
	{
		// todo: check for duplicate attribs
		attribBuf.push_back(Attrib{ uniform.MV * attribArr.vertices[idxBuf[i++]],
		                            uniform.N * attribArr.normals[idxBuf[i++]],
		                            attribArr.texCoords[idxBuf[i++]] });
	}
	// todo: tessellation
	assert(ctx->primMode==PrimMode::triangles);
	
	AttribBuffer clipBuf;
	for (int i=0; i<attribBuf.size()-2;)
	{
		{
			Attrib& v0 = attribBuf[i++];
			Attrib& v1 = attribBuf[i++];
			Attrib& v2 = attribBuf[i++];

			Vec4& p0 = v0.vertex;
			Vec4& p1 = v1.vertex;
			Vec4& p2 = v2.vertex;

			// discard triangle facing away from eye
			if (!isFrontFace(ctx->cullMode, ctx->faceMode, p0.xy(), p1.xy(), p2.xy()))
				continue;
			// project vertices
			p0 = uniform.P * p0;
			p1 = uniform.P * p1;
			p2 = uniform.P * p2;
			
			clipBuf = {v0, v1, v2};
		}
		// cull or clip partially visible triangles
		if (!clipPolygon(clipBuf))
			continue;
		assert(clipBuf.size()>=3);
		// draw clipped triangles
		for (int j=0; j<clipBuf.size()-2;)
		{
			Attrib& v0 = clipBuf[j++];
			Attrib& v1 = clipBuf[j++];
			Attrib& v2 = clipBuf[j++];
			
			Vec4& p0 = v0.vertex;
			Vec4& p1 = v1.vertex;
			Vec4& p2 = v2.vertex;
			
			// perspective divide
			assert(p0.w!=0&&p1.w!=0&&p2.w!=0);
			p0 *= 1.f/p0.w;
			p1 *= 1.f/p1.w;
			p2 *= 1.f/p2.w;
			// convert to window space
			p0 = uniform.W * p0;
			p1 = uniform.W * p1;
			p2 = uniform.W * p2;

			assert(ctx->drawMode & DrawBit::fill); // debugging
			// rasterize triangle
			drawTriangle(ctx, v0, v1, v2);
		}
	}
}

void gl::drawTriangle(Context* ctx, const Attrib& v0, const Attrib& v1, const Attrib& v2)
{
	const Vec4& p0 = v0.vertex;
	const Vec4& p1 = v1.vertex;
	const Vec4& p2 = v2.vertex;
	FrameBuffer& frameBuf = *ctx->frameBuf;
	const Uniform& uniform = *ctx->uniform;

	// compute AABB
	const float x0 = float(std::max(int(std::min(p2.x, std::min(p0.x, p1.x))), 0));
	const float y0 = float(std::max(int(std::min(p2.y, std::min(p0.y, p1.y))), 0));
	
	const float x1 = float(std::min(int(std::max(p2.x, std::max(p0.x, p1.x))), int(frameBuf.width-1)));
	const float y1 = float(std::min(int(std::max(p2.y, std::max(p0.y, p1.y))), int(frameBuf.height-1)));

	// compute area of entire triangle for "normalizing" bary coords
	const float area = edge(p0.xy(), p1.xy(), p2.xy());
	assert(area!=0.f);
	const float oneOverArea = 1.f/area;

	Vec2 p{x0, y0};
	// compute bary coords only once for first frag
	float Wy0 = edge(p1.xy(), p2.xy(), p);
	float Wy1 = edge(p2.xy(), p0.xy(), p);
	float Wy2 = area - Wy0 - Wy1;
	// bary coords are shifted by delta amount for every frag
	// TODO: bad, use fixed-point step deltas
	const float xStep0 = p1.y - p2.y, yStep0 = p2.x - p1.x;
	const float xStep1 = p2.y - p0.y, yStep1 = p0.x - p2.x;
	const float xStep2 = p0.y - p1.y, yStep2 = p1.x - p0.x;
	// pre-compute constant values in interpolation equations
	const float z1 = (p1.z - p0.z)*oneOverArea;
	const float z2 = (p2.z - p0.z)*oneOverArea;
	const Vec2 uv1 = (v1.texCoord - v0.texCoord)*oneOverArea;
	const Vec2 uv2 = (v2.texCoord - v0.texCoord)*oneOverArea;

	for (p.y=y0; p.y<=y1; ++p.y, Wy0+=yStep0, Wy1+=yStep1, Wy2+=yStep2)
	{
		unsigned int idx = frameBuf.width*p.y+x0;
		float Wx0 = Wy0;
		float Wx1 = Wy1;
		float Wx2 = Wy2;
		for (p.x=x0; p.x<=x1; ++p.x, ++idx, Wx0+=xStep0, Wx1+=xStep1, Wx2+=xStep2)
		{
			// discard frag if not inside of the triangle
			if (Wx0<0 || Wx1<0 || Wx2<0)
				continue;
			
			// interpolate depth using bary coords
			const float z = p0.z + Wx1*z1 + Wx2*z2;
			// discard frag if a closer frag is stored in this depth-buffer position
			if (z >= frameBuf.depthBuf[idx])
				continue;
			frameBuf.depthBuf[idx] = z;

			// interpolate texture coordinates
			gmath::Vec2 interpUv = v0.texCoord + uv1*Wx1 + uv2*Wx2;
			// halfway vector between camera dir and light dir
			gmath::Vec3 halfDir = gmath::normalize(gmath::Vec3{0,0,-1} + uniform.lightDir);
			// sample diffuse texture
			gmath::Vec3 sampleRgb = uniform.tex2d->sample(interpUv, gl::TextureType::diffuse);
			// sample normal texture
			gmath::Vec3 sampleN = uniform.tex2d->sample(interpUv, gl::TextureType::normal);
			gmath::Vec3 frag{
				// ambient
				sampleRgb * (uniform.lightAmbient
				// diffuse
				+ uniform.lightIntensity * std::max(0.f, gmath::dot(sampleN, uniform.lightDir)))
				// specular
				+ uniform.lightSpecRgb * uniform.lightIntensity
				* powf(
					gmath::dot(gmath::normalize(uniform.P * uniform.N * sampleN), halfDir),
					uniform.lightSpecExp)
			};
			
			frameBuf.colorBuf[idx] = Vec3(clamp(frag.r, 0.f, 1.f),
			                              clamp(frag.g, 0.f, 1.f),
			                              clamp(frag.b, 0.f, 1.f));
		}
	}
}
