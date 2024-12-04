cmake_minimum_required(VERSION 3.30)

project(cereal
    VERSION 0.0.1
    DESCRIPTION "A C++ binary serialization library"
    HOMEPAGE_URL "https://github.com/MumblyJuergens/cereal"
    LANGUAGES CXX
)

include(cmake/CPM.cmake)
CPMAddPackage("gh:MumblyJuergens/mj#main")

set(cereal_headers
    "include/cereal/cereal.hpp"
)

set(cereal_src
    "src/cereal.cpp"
)

add_library(cereal ${cereal_src} ${cereal_headers})
add_library(cereal::cereal ALIAS cereal)
target_compile_features(cereal PUBLIC cxx_std_26)
target_include_directories(cereal PUBLIC "include")
target_link_libraries(cereal PUBLIC mj::mj)
MJFullCompilerWarnings(cereal)

if (PROJECT_IS_TOP_LEVEL)
    add_subdirectory("tests")
endif()