# Vainmoinen
A software rasterizer which rasterizes perspectively-corrected textured triangles.

Disclaimer: I developed this project back in 2011-2012. My coding and analysis skills were definitely less so back then. Because of that take any claims and performance numbers below with a grain of salt.

## Update: 18th of June, 2012

After a long brake I decided to update the project, mostly with aim to learn some new stuff and try new ideas. There are three important changes. First one is that the renderer can now use not only CUDA to speed up the computations but also DirectCompute. Secondly, the CUDA-supported renderer does not perform any copy of the final image to the CPU; the final rasterized picture is rendered directly from the GPU memory using OpenGL interoperability (PBO and texture buffer objects). The third major change is the way the GPU-based rasterization is done.

Previous version of Vainmoinen sliced each triangle into tiles of 16x16 pixels. Two variations were available. The first one processed each triangle with a single CUDA kernel call. The second one could batch more triangles using sort of "virtual tiling". This way up to 65535 triangles' tiles could be processed with a single kernel call. The main problem with this approach turned out to be synchronization. Since triangles can overlap in screen-space, so can the tiles they are made of. This caused occasional flickering of pixels due to race condition.

The new hardware rasterization algorithm also uses tiling approach but now the screen is tiled instead of triangles. Each tile is assigned to a unique portion of the screen occupying 16x16 pixels area. The renderer can schedule up to a fixed number of triangles to be processed with a single kernel call. The data of triangles that are to be rendered in a particular kernel call are stored in GPU memory (and are submitted there by the CPU every frame). Moreover, for each screen's tile there is a list of indices to those triangles that affect the tile in mind. The algorithm simply iterates over the triangles and rasterizes their pixels.

### New features
- supports CUDA as well as DirectCompute (both depict *very* comparable performance)
- configurable number of triangles to be processed in a single kernel call
- CUDA: can use constant memory for storing triangles' data
- CUDA: can use streams to parallelize computations with data transfers

### Performance

One test was conducted. Three full-screen quads were rendered on top of one another (sorted back-to-front so no early-z). The screen resolution was 1366x768. The GPU used was GeForce GT540M. Performance stats:
| OpenGL  | CUDA |
| ------------- | ------------- |
| 1.163ms | clear buffers (kernel) - 0.412ms <br/> triangles buffer (copy) - 0.001ms <br/> tiles' indices to triangles buffer (copy) - 1.500ms <br/> rasterize (kernel) - 8.653ms |

OpenGL's timing includes the whole pipeline. However, since there were only 12 vertices to process, the time needed for vertex processing can be neglected.
GL_ARB_timer_query extension was used to measure the OpenGL's time. NVIDIA's Compute Visual Profiler was used to get CUDA's timings.

### Downloads
- [Vainmoinen-18-06-2012](http://maxest.gct-game.net/content/vainmoinen/Vainmoinen-18-06-2012.zip) (includes sources, project files for Visual C++ 2008, third-party dependencies, samples)

### Remarks
- CUDA: v4.0 was used
- CUDA: at first, instead of using PBO and texture, PBO alone was used and the final image was rendered with glDrawPixels. Supposingly this function caused some CPU <-> GPU traffic, as it took around 7ms to complete, thus degrading the performance severly (rendering was 2x slower with respect to DirectCompute)
- CUDA: I was unable to use my own math library for vectors so I created special types like vec2_CUDA, vec3_CUDA and vec4_CUDA. The reason for this were some problems with CUDA compiler (does not support full C++ functionality), nested types declarations (TriangleToRasterize structure uses vectors) and constructors
- CUDA: I could not use CUDA's built-in float2/float3/float4 types because they imposed some weird alignment and didn't fit well into my data structures

### Issues
- CUDA, DirectCompute: when camera is far from a rasterized triangle, the triangle's pixels flicker
- CUDA: the renderer uses OpenGL's PBO and texture buffer objects.
- CUDA fills data in the GPU memory, which is also pointed to by OpenGL's pixel buffer object. The code that sets this upon initialization looks like this:
  - create PBO
  - call cudaGraphicsGLRegisterBuffer to register a CUDA buffer that will point to PBO
  - create texture
- It turns out that if step 2. is performed after step 3., the performance of the application drops, what is a mystery to me why this happens. Maybe a CUDA driver bug?


## 1st of February, 2011

Vainmoinen is a project that has been developed for my bachelor thesis "Software Renderer Accelerated by CUDA Technology". As can be easily concluded from the thesis's title, Vainmoinen is a software renderer that can use CUDA technology to speed up the rendering. The renderer supports a very small selected subset of OpenGL/Direct3D functionality. Here's the list:
- vertex transformation
- triangle culling and clipping
- object-order rasterization and shading with depth buffering
- texture mapping supporting bilinear filtering and mip-mapping (not available for CUDA renderer)

### Downloads
- [Vainmoinen-20-02-2011](http://maxest.gct-game.net/content/vainmoinen/Vainmoinen-20-02-2011.zip) (includes sources, project files for Visual C++ 2008, third-party dependencies, samples)
- [bachelor thesis](http://maxest.gct-game.net/content/vainmoinen/bachelor_thesis.pdf) "Software Renderer Accelerated by CUDA Technology"
- [bachelor thesis defence](http://maxest.gct-game.net/content/vainmoinen/bachelor_thesis_defence.pdf) slides

### Remarks
- CUDA v3.2 was used during development. CUDA v4.0 depicts some problems during compilation
