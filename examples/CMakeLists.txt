set(src
    sample_01.cc
    sample_02.cc
)

add_executable(sample_01 sample_01.cc)
target_link_libraries(sample_01 PRIVATE vita::vita vita::project_options vita::project_warnings)

add_executable(sample_02 sample_02.cc)
target_link_libraries(sample_02 PRIVATE vita::vita vita::project_options vita::project_warnings)

source_group("" FILES ${src})
cmake_source_group()
