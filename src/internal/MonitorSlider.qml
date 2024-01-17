// SPDX-License-Identifier: LGPL-3.0-or-later

import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

// NOTE: All the values here make monitor sliders look exactly
// like on Scratch, so be careful when doing any changes.
Slider {
    id: control
    property bool discrete: false

    QtObject {
        id: priv
        readonly property color bgColor: Qt.rgba(0.94, 0.94, 0.94, 1)
        readonly property color currentBgColor: control.pressed ? Qt.rgba(0.96, 0.96, 0.96, 1) : (hoverHandler.hovered ? Qt.darker(bgColor, 1.28) : bgColor)

        readonly property color bgBorderColor: Qt.rgba(0.7, 0.7, 0.7, 1)
        readonly property color currentBgBorderColor: control.pressed ? Qt.rgba(0.77, 0.77, 0.77, 1) : (hoverHandler.hovered ? Qt.darker(bgBorderColor, 1.28) : bgBorderColor)

        readonly property color sliderColor: Qt.rgba(0, 0.46, 1, 1)
        readonly property color currentSliderColor: control.pressed ? Qt.rgba(0.22, 0.58, 1, 1) : (hoverHandler.hovered ? Qt.darker(sliderColor, 1.28) : sliderColor)

        readonly property color positionBorderColor: Qt.rgba(0.21, 0.46, 0.75, 1)
        readonly property color currentPositionBorderColor: control.pressed ? Qt.rgba(0.37, 0.56, 0.79, 1) : (hoverHandler.hovered ? Qt.darker(positionBorderColor, 1.28) : positionBorderColor)
    }

    stepSize: discrete ? 1 : 0.1
    snapMode: Slider.SnapAlways
    implicitWidth: 119
    implicitHeight: 16
    leftPadding: -4
    rightPadding: -4
    topPadding: -1
    bottomPadding: 0
    activeFocusOnTab: false
    hoverEnabled: true

    background: Rectangle {
        x: control.leftPadding
        y: control.topPadding + control.availableHeight / 2 - height / 2
        implicitWidth: 200
        implicitHeight: 7.5
        width: control.availableWidth
        height: implicitHeight
        radius: 6
        color: priv.currentBgColor
        border.color: priv.currentBgBorderColor

        Rectangle {
            width: control.visualPosition * (parent.width - handle.width) + handle.width
            height: parent.height
            color: priv.currentSliderColor
            border.color: priv.currentPositionBorderColor
            radius: 6
        }
    }

    handle: Rectangle {
        x: control.leftPadding + control.visualPosition * (control.availableWidth - width)
        y: control.topPadding + control.availableHeight / 2 - height / 2
        implicitWidth: 15
        implicitHeight: 15
        radius: 7
        color: control.pressed ? priv.currentSliderColor : (handleHoverHandler.hovered ? priv.currentSliderColor : priv.sliderColor)

        HoverHandler {
            id: handleHoverHandler
        }
    }

    HoverHandler {
        id: hoverHandler
    }
}
