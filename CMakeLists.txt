cmake_minimum_required(VERSION 3.14)

project(scratchcpp-render VERSION 0.9.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(SCRATCHCPPRENDER_BUILD_UNIT_TESTS "Build unit tests" ON)

if(EMSCRIPTEN)
    set(CMAKE_C_FLAGS "-pthread")
    set(CMAKE_CXX_FLAGS "-pthread")
endif()

include(build/FindQt.cmake)

qt_standard_project_setup(REQUIRES 6.6)
add_subdirectory(src)

target_compile_definitions(scratchcpp-render PRIVATE SCRATCHCPPRENDER_LIBRARY)
target_compile_definitions(scratchcpp-render PRIVATE SCRATCHCPPRENDER_VERSION="${PROJECT_VERSION}")
target_compile_definitions(scratchcpp-render PRIVATE SCRATCHCPPRENDER_VERSION_MAJOR=${PROJECT_VERSION_MAJOR})
target_compile_definitions(scratchcpp-render PRIVATE SCRATCHCPPRENDER_VERSION_MINOR=${PROJECT_VERSION_MINOR})
target_compile_definitions(scratchcpp-render PRIVATE SCRATCHCPPRENDER_VERSION_PATCH=${PROJECT_VERSION_PATCH})

linkQt(scratchcpp-render)

target_sources(scratchcpp-render
  PUBLIC
    include/scratchcpp-render/scratchcpp-render.h
)

target_include_directories(scratchcpp-render PUBLIC include)

include(build/SetUpLibscratchcpp.cmake)
target_link_libraries(scratchcpp-render PRIVATE scratchcpp)

add_subdirectory(thirdparty/libqnanopainter)
target_include_directories(scratchcpp-render PRIVATE thirdparty/libqnanopainter)
target_link_libraries(scratchcpp-render PRIVATE qnanopainter)

if (SCRATCHCPPRENDER_BUILD_UNIT_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()
