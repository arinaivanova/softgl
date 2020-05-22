#include <iostream>
#include <cmath>
#include "SOFTGL/gl.h"
#include "GMATH/transform.h"

int main(int argc, char *argv[]){
	// INDEX BUFFER
	gl::IndexBuffer idx;
	// ATTRIB ARRAYS
	gl::AttribArrays vb;
	// TODO: load model files and fill attrib arrays and index buffer
	// ex: vb.texCoords.push_back(gmath::Vec2{u, v});
	// NOTE: mesh must have vertices, normals, and texture coordinates

	// FRAMEBUFFER
	gl::FrameBuffer fb(1000,800);
	// init buffers
	fb.clear(gl::BufBit::color | gl::BufBit::depth);

	// VIEWPORT SETTINGS
	const float n = 0.1f;
	const float f = 1000.f;
	const float fov = 0.3f;
	const float ar = fb.aspect;
	const float t = tanf(fov*0.5f)*n, b = -t;
	const float r = t*ar,             l = -r;

	// UNIFORM
	gl::Uniform uni;
	// set up matrices
	uni.M =  gmath::translate(0,0,0)
			*gmath::scale(0.7f, 0.7f, 0.7f)
			*gmath::rotate(0,-1.6,0);
	uni.V = gmath::lookatEuler(gmath::Vec3{0,0,7.5}, 0.f, 0.f, 0.f);
	uni.P = gmath::perspective(f, n, b, t, l, r);
	uni.W = gmath::viewport(0.f, 0.f, fb.width, fb.height, n, f);
	uni.N  = gmath::transpose(gmath::inverse(uni.M));
	uni.MV = uni.V * uni.M;

	// LIGHTING SETTINGS
	uni.lightDir = gmath::Vec3{0,0,0.9};
	uni.lightSpecRgb = gmath::Vec3{1,1,1};
	uni.lightAmbient = 0.3f;
	uni.lightIntensity = 1.1f;
	uni.lightSpecExp = 128;
	
	// SAMPLER
	gl::Sampler2D tex2d;
	tex2d.channels = 3;
	// TODO: load textures
	// bind sampler to uniform
	uni.tex2d = &tex2d;

	// PIPELINE SETTINGS
	gl::Context ctx;
	ctx.faceMode = gl::FrontFace::CCW;
	ctx.cullMode = gl::CullBit::back;
	ctx.drawMode = gl::DrawBit::fill;
	ctx.imageMode = gl::BufBit::depth | gl::BufBit::color;

	// bind the buffers
	ctx.attribArr = &vb;
	ctx.frameBuf = &fb;
	ctx.uniform = &uni;
	// DRAW PRIMITIVES using index buffer
	gl::drawIndexedArrays(&ctx, idx);
	// result stored in framebuffer object
	
	return 0;
}
