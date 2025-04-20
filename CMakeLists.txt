cmake_minimum_required(VERSION 3.29.4)

cmake_policy(SET CMP0077 NEW)
cmake_policy(SET CMP0069 NEW) # enforce INTERPROCEDURAL_OPTIMIZATION

project(Solution
    VERSION 1.0.0
    DESCRIPTION "Your Project"
    HOMEPAGE_URL https://github.com/YourUser/YourProject
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "")
set(CMAKE_CXX_STANDARD 23)
if(!${MSVC})
    set(CMAKE_CXX_LINKER_LAUNCHER "/usr/bin/ccache")
    set(CMAKE_LINKER_TYPE MOLD)
else()
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.12.25835)
        set(CMAKE_CXX20_STANDARD_COMPILE_OPTION "-std:c++latest")
        set(CMAKE_CXX20_EXTENSION_COMPILE_OPTION "-std:c++latest")
    endif()
endif()
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    #set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-ftime-report")
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(libs)
add_subdirectory(src)


