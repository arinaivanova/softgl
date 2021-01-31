# softgl: Simple 3D Software Rasterizer
An educational project that aims to implement a graphics pipeline in C++. It takes 3D scene information, including vertex attributes, textures associated with the 3D models, and camera settings as input, processes and rasterizes the image entirely in software using its own [math library](https://github.com/arinaivanova/gmath), and produces a 2D raster image of the scene.

See [Source Code](https://github.com/arinaivanova/softgl/tree/master).

### Features
- [X] Custom camera support
- [X] Triangle mesh support
- [X] Texture mapping: diffuse, normal, specular
- [X] Homogeneous polygon clipping (Sutherland-Hodgman)
- [X] Perspective-correct interpolation
- [X] Depth buffering
- [X] Viewing frustum culling
- [X] Back-face culling

## Input and usage overview
Context stores information of the 3D scene, and frame buffer contains the output image.
```c++
	gl::Context context;
	gl::FrameBuffer image( width, height );
```
1. Add 3D scene information
```c++
	// Pool of vertex attributes associated with a 3D scene
	gl::AttribArrays attribArrays;
	
	// Add the three types of vertex attributes to arrays
	attribArrays.vertices.push_back( gmath::Vec4{ x, y, z, w } );	  // position point in space
  	attribArrays.normals.push_back( gmath::Vec3{ x, y, w } ); 	  // normal vector
	attribArrays.texCoords.push_back( gmath::Vec2{ u, v } ); 	  // point on the texture map
	
	// Add vertex attributes to the scene context
	context.attribArr = &attribArrays;
	
	// Create vertices that make up a 3D scene from the pool of vertex attributes
	gl::IndexBuffer indexBuffer;
	// Each vertex has an index to a position point in space, a normal vector and a point on the texture map
	indexBuffer.push_back(positionIndex), indexBuffer.push_back(normalIndex), indexBuffer.push_back(texCoordIndex);
```
2. Render a 2D image of the scene
```c++
	gl::drawIndexedArrays( &context, indexBuffer );
```
3. Access the rendered image
```c++
	for (int y = 0; y < height; y++) {
  		for (int x = 0; x < width; x++) {
			// RGB color of the pixel at (x,y)
 			gmath::Vec3 pixelColor = image.colorBuf[ y * width + x ];
 		}
 	}
```
See [full sample input code](https://github.com/arinaivanova/softgl/blob/436062143b279ffde794a496812d56b9f8cb6c11/example.cpp#L6).

## Sample output
3D models were input to softgl render the following images:

<img src="https://github.com/arinaivanova/softgl/blob/master/render-1.png" alt="alt text" width="50%" height="50%"><img src="https://github.com/arinaivanova/softgl/blob/master/render-2.png" alt="alt text" width="50%" height="50%">

Models obtained from

[Samuel (arshlevon) Sharit (n.d.). Diablo 3 pose.](https://github.com/ssloy/tinyrenderer/tree/master/obj/diablo3_pose)

[Vidar Rapp (2007). Male african head example.](https://github.com/ssloy/tinyrenderer/tree/master/obj/african_head)
