embed(src_assets
    AS_TEXT
        vita/assets/lit.frag
        vita/assets/static.vert
    AS_BINARY
        vita/assets/uv.png
)

set(src_render
)
    
set(src_base
    vita/assert.cc vita/assert.h
    vita/dependency_glad.h
    vita/dependency_sdl.cc vita/dependency_sdl.h
    vita/result.cc vita/result.h
    vita/str.cc vita/str.h

    vita/anim/attribute.cc vita/anim/attribute.h
    vita/anim/draw.cc vita/anim/draw.h
    vita/anim/indexbuffer.cc vita/anim/indexbuffer.h
    vita/anim/mat4.cc vita/anim/mat4.h
    vita/anim/quat.cc vita/anim/quat.h
    vita/anim/shader.cc vita/anim/shader.h
    vita/anim/texture.cc vita/anim/texture.h
    vita/anim/transform.cc vita/anim/transform.h
    vita/anim/uniform.cc vita/anim/uniform.h
    vita/anim/vec2.h vita/anim/vec3.cc
    vita/anim/vec3.h vita/anim/vec4.h

    # chapter 08
    vita/anim/track.cc vita/anim/track.h
    vita/anim/transformtrack.cc vita/anim/transformtrack.h
    vita/anim/frame.h
    vita/anim/interpolation.h
    vita/anim/debugdraw.cc vita/anim/debugdraw.h
    
    vita/vita.cc vita/vita.h
    vita/opengl_utils.cc vita/opengl_utils.h
    vita/assets.cc vita/assets.h
    
    vita/compiler_warning.h
    vita/log.h
    vita/cint.h
    vita/undef_windows.h
)

set(src_pch
    vita/pch.public.h vita/pch.private.h
)

set(src
    ${src_render}
    ${src_base}
    ${src_pch}
    ${src_assets}
    ${shaders}
)

add_library(vita STATIC ${src})
target_link_libraries(vita
    PUBLIC
        external::sdl2
        external::opengl
        stb::image
        external::glm
        external::glad
        external::imgui
    PRIVATE
        embed::embed
        vita::project_options
        vita::project_warnings
)
target_include_directories(vita
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
)
target_precompile_headers(vita
    PUBLIC vita/pch.public.h
    PRIVATE vita/pch.private.h
)

source_group("" FILES ${src})

source_group("input" FILES ${src_input})
source_group("render" FILES ${src_render})
# source_group("base" FILES ${src_base})
source_group("pch" FILES ${src_pch})
source_group("_assets" FILES ${src_assets})
source_group("shaders" FILES ${shaders})

cmake_source_group()

add_library(vita::vita ALIAS vita)


set(src_test
    vita/main.test.cc

    vita/anim/mat4.test.cc
    vita/anim/track.test.cc
)
add_executable(test
    ${src_test}
)
target_link_libraries(test
    catchy::catchy
    vita::vita
)