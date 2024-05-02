set(src
    sample_01.cc
    sample_02.cc
)

add_executable(sample_01 sample_01.cc)
target_link_libraries(sample_01 PRIVATE vita::vita vita::project_options vita::project_warnings)

add_executable(sample_02 sample_02.cc)
target_link_libraries(sample_02 PRIVATE vita::vita vita::project_options vita::project_warnings)

add_executable(sample_03 sample_03.cc)
target_link_libraries(sample_03 PRIVATE vita::vita vita::project_options vita::project_warnings)

add_executable(sample_04 sample_04.cc)
target_link_libraries(sample_04 PRIVATE vita::vita vita::project_options vita::project_warnings)

add_executable(sample_05 sample_05.cc)
target_link_libraries(sample_05 PRIVATE vita::vita vita::project_options vita::project_warnings)

add_executable(sample_06 sample_06.cc)
target_link_libraries(sample_06 PRIVATE vita::vita vita::project_options vita::project_warnings)

add_executable(sample_07 sample_07.cc)
target_link_libraries(sample_07 PRIVATE vita::vita vita::project_options vita::project_warnings)

source_group("" FILES ${src})
cmake_source_group()
