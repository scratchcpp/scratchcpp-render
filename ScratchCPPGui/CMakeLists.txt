qt_add_library(libscratchcpp-gui STATIC)

set_target_properties(libscratchcpp-gui PROPERTIES AUTOMOC ON)

qt_add_qml_module(libscratchcpp-gui
    URI ScratchCPPGui
    VERSION 1.0
    QML_FILES
        ProjectPlayer.qml
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
	irenderedtarget.h
        renderedtarget.cpp
        renderedtarget.h
	targetpainter.cpp
	targetpainter.h
	keyeventhandler.cpp
	keyeventhandler.h
)
