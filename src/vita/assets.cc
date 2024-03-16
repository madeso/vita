#include "vita/assets.h"

#include "lit.frag.h"
#include "static.vert.h"
#include "uv.png.h"

namespace assets
{
Source static_shader()
{
	return {std::string{STATIC_VERT}};
}

Source lit_shader()
{
	return {std::string{LIT_FRAG}};
}

TextureData uv_texture()
{
	return {static_cast<const void*>(UV_PNG.data), static_cast<int>(UV_PNG.size), "uv.png"};
}
}  //  namespace assets
