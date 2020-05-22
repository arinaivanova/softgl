// softgl gl.h
// Date: 09 02 2020
// Author: arinaivanova
// URL: https://github.com/arinaivanova/softgl
// Commentary: SoftGL core

#ifndef SOFTGL_GL_H_
#define SOFTGL_GL_H_

#include <vector>
#include <stdint.h>
#include "framebuffer.h"
#include "sampler.h"
#include "GMATH/mat4.h"
#include "GMATH/vec4.h"

namespace gl
{
	enum DrawBit : uint8_t
	{
		fill  = 1<<0,
		line  = 1<<1,
		point = 1<<2
	};

	enum CullBit : uint8_t
	{
		back  = 1<<0,
		front = 1<<1
	};
	
	enum PrimMode { triangles = 3U };

	enum FrontFace { CW, CCW };
	
	struct Attrib
	{
		gmath::Vec4 vertex;
		gmath::Vec3 normal;
		gmath::Vec2 texCoord;
	};

	struct AttribArrays
	{
		std::vector<gmath::Vec4> vertices;
		std::vector<gmath::Vec3> normals;
		std::vector<gmath::Vec2> texCoords;
	};

	typedef std::vector<Attrib> AttribBuffer;
	typedef std::vector<uint16_t> IndexBuffer;
	
	struct Uniform
	{
		gmath::Mat4 M, V, P, W, N, MV;
		gmath::Vec3 lightDir;
		gmath::Vec3 lightSpecRgb;
		float lightIntensity;
		float lightAmbient;
		int lightSpecExp;
		Sampler2D* tex2d;
	};
	
	struct Context
	{
		const AttribArrays* attribArr;
		const Uniform* uniform;
		FrameBuffer* frameBuf;
		// rendering settings
		PrimMode primMode  {PrimMode::triangles};
		FrontFace faceMode {FrontFace::CCW};
		uint8_t drawMode;
		uint8_t cullMode;
		uint8_t imageMode;
	};
	
	void drawIndexedArrays(Context* ctx, const IndexBuffer& idxBuf);

	void drawTriangle(Context* ctx, const Attrib& v0, const Attrib& v1, const Attrib& v2);

} // namespace gl

#endif
