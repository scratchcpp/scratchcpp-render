cmake_minimum_required(VERSION 3.14)

project(scratchcpp-gui VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(build/FindQt.cmake)

qt_standard_project_setup(REQUIRES 6.6)
add_subdirectory(ScratchCPPGui)

set(QML_IMPORT_PATH "${QML_IMPORT_PATH};${PROJECT_SOURCE_DIR}/src/qml" PARENT_SCOPE)

target_compile_definitions(libscratchcpp-gui PRIVATE SCRATCHCPPGUI_LIBRARY)

target_link_libraries(libscratchcpp-gui PRIVATE Qt6::Gui Qt6::Qml Qt6::Quick)

include(build/SetUpLibscratchcpp.cmake)
target_link_libraries(libscratchcpp-gui PRIVATE scratchcpp)

add_subdirectory(thirdparty/libqnanopainter)
target_include_directories(libscratchcpp-gui PRIVATE thirdparty/libqnanopainter)
target_link_libraries(libscratchcpp-gui PRIVATE qnanopainter)
