// SPDX-License-Identifier: LGPL-3.0-or-later

import QtQuick
import QtQuick.Controls

Rectangle {
    id: root
    property string question: ""
    readonly property alias answer: textField.text
    signal closed()

    QtObject {
        id: priv
        readonly property int margin: 18
        readonly property int spacing: 6
    }

    color: "white"
    border.color: Qt.rgba(0.85, 0.85, 0.85, 1)
    border.width: 2
    radius: 9
    implicitHeight: labelLoader.height + textField.height + 2 * priv.margin + priv.spacing
    onActiveFocusChanged: {
        if(activeFocus)
            textField.forceActiveFocus();
    }

    function clear() {
        textField.clear();
        question = "";
    }

    Loader {
        id: labelLoader
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.leftMargin: priv.margin
        anchors.topMargin: priv.margin
        anchors.rightMargin: priv.margin
        active: root.question !== ""

        sourceComponent: Text {
            text: root.question
            color: "#575E75"
            font.family: "Helvetica"
            font.pointSize: 9
            font.bold: true
        }
    }

    TextField {
        id: textField
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: labelLoader.bottom
        anchors.leftMargin: priv.margin - leftInset
        anchors.topMargin: priv.spacing - topInset
        anchors.rightMargin: priv.margin - rightInset
        anchors.bottomMargin: priv.margin - bottomInset
        color: "#575E75"
        font.family: "Helvetica"
        font.pointSize: 8
        leftInset: 5
        topInset: 0
        rightInset: 0
        bottomInset: 0
        padding: 0
        Keys.onReturnPressed: closed()
        Keys.onEnterPressed: closed()

        background: Rectangle {
            color: root.color
            border.color: root.border.color
            radius: height / 2
            implicitHeight: 30
        }

        Rectangle {
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.rightMargin: 4
            anchors.topMargin: 3.5
            color: "#4D97FF"
            width: 25
            height: 25
            radius: width / 2

            Image {
                anchors.fill: parent
                source: "qrc:/qt/qml/ScratchCPP/Render/icons/enter.svg"

                MouseArea {
                    anchors.fill: parent
                    onClicked: closed()
                }
            }
        }
    }
}
