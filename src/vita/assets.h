#pragma once

#include "vita/anim/shader.h"
#include "vita/anim/texture.h"
#include "vita/anim/gltfloader.h"

namespace assets
{
ShaderSource static_shader();
ShaderSource lit_shader();

TextureData uv_texture();

GltfFile woman_gltf();


}  //  namespace assets
