find_package(Qt6 6.6 COMPONENTS Gui Qml Quick REQUIRED)
set(QT_LIBS Qt6::Gui Qt6::Qml Qt6::Quick)

function(linkQt TARGET)
    target_link_libraries(${TARGET} PRIVATE ${QT_LIBS})
endfunction()
