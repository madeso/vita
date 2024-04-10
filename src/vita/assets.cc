#include "vita/assets.h"

#include "lit.frag.h"
#include "static.vert.h"
#include "uv.png.h"
#include "woman.gltf.h"

namespace assets
{
ShaderSource static_shader()
{
	return {std::string{STATIC_VERT}};
}

ShaderSource lit_shader()
{
	return {std::string{LIT_FRAG}};
}

TextureData uv_texture()
{
	return {static_cast<const void*>(UV_PNG.data), static_cast<int>(UV_PNG.size), "uv.png"};
}

GltfFile woman_gltf()
{
	return {"woman.gltf", WOMAN_GLTF.data, WOMAN_GLTF.size};
}

}  //  namespace assets
