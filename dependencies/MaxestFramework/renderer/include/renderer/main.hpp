#ifndef RENDERER_H
#define RENDERER_H



#ifdef RENDERER_D3D11
	#include <renderer_d3d11/renderer_d3d11.hpp>
#elif RENDERER_OGL
	#include <renderer_ogl/renderer_ogl.hpp>
	#include <renderer_ogl/vertex_declaration_ogl.hpp>
	#include <renderer_ogl/vertex_buffer_ogl.hpp>
	#include <renderer_ogl/index_buffer_ogl.hpp>
	#include <renderer_ogl/shader_ogl.hpp>
	#include <renderer_ogl/texture_ogl.hpp>
#endif



#endif
