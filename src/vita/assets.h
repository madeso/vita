#pragma once

#include "vita/anim/shader.h"
#include "vita/anim/texture.h"
#include "vita/anim/gltfloader.h"

namespace assets
{
ShaderSource static_shader();
ShaderSource lit_shader();
ShaderSource skinned_shader();

TextureData uv_texture();
TextureData woman_texture();

GltfFile woman_gltf();
GltfFile ik_course_gltf();


}  //  namespace assets
