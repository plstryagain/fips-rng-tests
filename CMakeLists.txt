cmake_minimum_required(VERSION 3.0)
project(fipsrngtests C)


include_directories(include)
add_subdirectory(src)

if(BUILD_TEST)
    add_subdirectory(tests)
endif(BUILD_TEST)

add_library(fipsrngtests STATIC 
    $<TARGET_OBJECTS:fips>
)
