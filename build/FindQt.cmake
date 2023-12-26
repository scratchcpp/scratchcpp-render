find_package(Qt6 6.6 COMPONENTS Gui Qml Quick Svg REQUIRED)
set(QT_LIBS Qt6::Gui Qt6::Qml Qt6::Quick Qt6::Svg)

if (SCRATCHCPPRENDER_BUILD_UNIT_TESTS)
    find_package(Qt6 6.6 COMPONENTS Test REQUIRED)
endif()

function(linkQt TARGET)
    target_link_libraries(${TARGET} PRIVATE ${QT_LIBS})
endfunction()
