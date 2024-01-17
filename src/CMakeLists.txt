qt_add_library(scratchcpp-render STATIC)

set_target_properties(scratchcpp-render PROPERTIES AUTOMOC ON)

qt_add_qml_module(scratchcpp-render
    URI ScratchCPP.Render
    VERSION 1.0
    OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/ScratchCPP/Render
    QML_FILES
        ProjectPlayer.qml
    RESOURCES
        internal/ValueMonitor.qml
	internal/MonitorSlider.qml
    SOURCES
        global.h
	projectloader.cpp
	projectloader.h
        projectscene.cpp
        projectscene.h
	stagemodel.cpp
	stagemodel.h
	spritemodel.cpp
	spritemodel.h
	monitormodel.cpp
	monitormodel.h
	valuemonitormodel.cpp
	valuemonitormodel.h
	listmonitorlistmodel.cpp
	listmonitorlistmodel.h
	irenderedtarget.h
        renderedtarget.cpp
        renderedtarget.h
	targetpainter.cpp
	targetpainter.h
	scenemousearea.cpp
	scenemousearea.h
	mouseeventhandler.cpp
	mouseeventhandler.h
	keyeventhandler.cpp
	keyeventhandler.h
)

list(APPEND QML_IMPORT_PATH ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
list(REMOVE_DUPLICATES QML_IMPORT_PATH)
set(QML_IMPORT_PATH ${QML_IMPORT_PATH} CACHE STRING "" FORCE)
