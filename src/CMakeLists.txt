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
	internal/ListMonitor.qml
	internal/TextBubble.qml
	internal/Question.qml
	shaders/sprite.vert
	shaders/sprite.frag
	icons/enter.svg
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
	listmonitormodel.cpp
	listmonitormodel.h
	listmonitorlistmodel.cpp
	listmonitorlistmodel.h
	irenderedtarget.h
	texture.cpp
	texture.h
	skin.cpp
	skin.h
	bitmapskin.cpp
	bitmapskin.h
	svgskin.cpp
	svgskin.h
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
	ipenlayer.h
	penlayer.cpp
	penlayer.h
	penlayerpainter.cpp
	penlayerpainter.h
	penattributes.h
	penstate.h
	shadermanager.cpp
	shadermanager.h
	graphicseffect.cpp
	graphicseffect.h
	textbubbleshape.cpp
	textbubbleshape.h
	textbubblepainter.cpp
	textbubblepainter.h
	blocks/penextension.cpp
	blocks/penextension.h
	blocks/penblocks.cpp
	blocks/penblocks.h
)

list(APPEND QML_IMPORT_PATH ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
list(REMOVE_DUPLICATES QML_IMPORT_PATH)
set(QML_IMPORT_PATH ${QML_IMPORT_PATH} CACHE STRING "" FORCE)
