set(src
    main.cc
)

add_executable(example ${src})
source_group("" FILES ${src})
cmake_source_group()
target_link_libraries(example
    PRIVATE
        vita::vita
        vita::project_options
        vita::project_warnings
)