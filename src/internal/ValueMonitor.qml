// SPDX-License-Identifier: LGPL-3.0-or-later

import QtQuick
import QtQuick.Layouts
import ScratchCPP.Render

// NOTE: All the values here make monitors look exactly
// like on Scratch, so be careful when doing any changes.
Rectangle {
    property ValueMonitorModel model: null

    color: model ? (model.mode === ValueMonitorModel.Large ? model.color : priv.bgColor) : priv.bgColor
    border.color: Qt.rgba(0.765, 0.8, 0.85, 1)
    radius: 5
    width: layout.implicitWidth + priv.horizontalMargins * 2
    height: layout.implicitHeight + priv.verticalMargins * 2
    visible: model ? model.visible : true

    QtObject {
        id: priv
        readonly property int horizontalMargins: 9
        readonly property double verticalMargins: 2
        readonly property color bgColor: Qt.rgba(0.9, 0.94, 1, 1)
    }

    ColumnLayout {
        id: layout
        anchors.leftMargin: priv.horizontalMargins
        anchors.rightMargin: priv.horizontalMargins
        anchors.topMargin: priv.verticalMargins
        anchors.bottomMargin: priv.verticalMargins
        anchors.centerIn: parent
        spacing: 0

        Loader {
            active: model ? model.mode !== ValueMonitorModel.Large : true
            sourceComponent: RowLayout {
                spacing: 9

                Text {
                    color: Qt.rgba(0.34, 0.37, 0.46, 1)
                    text: model ? model.name : ""
                    textFormat: Text.PlainText
                    font.pointSize: 9
                    font.bold: true
                    font.family: "Helvetica"
                }

                Text {
                    id: valueText
                    color: "white"
                    text: model ? model.value : ""
                    textFormat: Text.PlainText
                    font.pointSize: 9
                    font.family: "Helvetica"
                    leftPadding: 2
                    rightPadding: 2
                    topPadding: -2
                    bottomPadding: -1
                    horizontalAlignment: Qt.AlignHCenter
                    Layout.minimumWidth: 40

                    Rectangle {
                        color: model ? model.color : "green"
                        radius: 5
                        anchors.fill: parent
                        anchors.margins: 0
                        z: -1
                    }
                }
            }
        }

        Loader {
            active: model ? model.mode === ValueMonitorModel.Slider : false
            sourceComponent: MonitorSlider {
                from: model ? model.sliderMin : 0
                to: model ? model.sliderMax : 0
                discrete: model ? model.discrete : true
                value: model ? model.value : 0
                Layout.fillWidth: true
                onMoved: {
                    if(model)
                        model.value = value;
                }
            }
        }

        Loader {
            active: model ? model.mode === ValueMonitorModel.Large : false
            sourceComponent: Text {
                color: "white"
                text: model ? model.value : ""
                textFormat: Text.PlainText
                font.pointSize: 12
                font.family: "Helvetica"
                leftPadding: 2
                rightPadding: 2
                topPadding: -3
                bottomPadding: 0
                horizontalAlignment: Qt.AlignHCenter
                width: Math.max(31, implicitWidth)
                height: Math.max(14.46, implicitHeight)
            }
        }
    }
}
