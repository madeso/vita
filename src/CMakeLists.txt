embed(src_assets
)

set(src_render
)
    
set(src_base
    anim/assert.cc anim/assert.h
    anim/dependency_glad.h
    anim/dependency_sdl.cc anim/dependency_sdl.h
    anim/result.cc anim/result.h
    anim/str.cc anim/str.h
    
    anim/anim.cc anim/anim.h
    anim/opengl_utils.cc anim/opengl_utils.h
    
    anim/compiler_warning.h
    anim/log.h
    anim/cint.h
    anim/undef_windows.h
)

set(src_pch
    anim/pch.public.h anim/pch.private.h
)

set(src
    ${src_render}
    ${src_base}
    ${src_pch}
    ${src_assets}
    ${shaders}
)

add_library(anim STATIC ${src})
target_link_libraries(anim
    PUBLIC
        external::sdl2
        external::opengl
        stb::image
        external::glm
        external::glad
        external::imgui
    PRIVATE
        embed::embed
        anim::project_options
        anim::project_warnings
)
target_include_directories(anim
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
)
target_precompile_headers(anim
    PUBLIC anim/pch.public.h
    PRIVATE anim/pch.private.h
)

source_group("" FILES ${src})

source_group("input" FILES ${src_input})
source_group("render" FILES ${src_render})
# source_group("base" FILES ${src_base})
source_group("pch" FILES ${src_pch})
source_group("_assets" FILES ${src_assets})
source_group("shaders" FILES ${shaders})

cmake_source_group()

add_library(anim::anim ALIAS anim)


set(src_test
    anim/main.test.cc
)
add_executable(test
    ${src_test}
)
target_link_libraries(test
    catchy::catchy
    anim::anim
)