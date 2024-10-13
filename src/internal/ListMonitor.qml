// SPDX-License-Identifier: LGPL-3.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import ScratchCPP.Render

// NOTE: All the values here make list monitors look
// like on Scratch, so be careful when doing any changes.
Rectangle {
    id: root
    property ListMonitorModel model: null

    width: model ? (model.width > 0 ? model.width : priv.defaultWidth) : priv.defaultWidth
    height: model ? (model.height > 0 ? model.height : priv.defaultHeight) : priv.defaultHeight
    color: Qt.rgba(0.9, 0.94, 1, 1)
    border.color: Qt.rgba(0.765, 0.8, 0.85, 1)
    radius: 5
    visible: model ? model.visible : true
    onWidthChanged: if(model) model.width = width
    onHeightChanged: if(model) model.height = height

    QtObject {
        id: priv
        readonly property int defaultWidth: 102
        readonly property int defaultHeight: 203
        readonly property color textColor: Qt.rgba(0.34, 0.37, 0.46, 1)
    }

    Text {
        id: header
        anchors.left: parent.left
        anchors.top: parent.top
        width: parent.width
        color: priv.textColor
        text: model ? model.name : ""
        textFormat: Text.PlainText
        font.pointSize: 9
        font.bold: true
        font.family: "Helvetica"
        horizontalAlignment: Qt.AlignHCenter
        wrapMode: Text.WordWrap
        clip: true
        padding: 3

        Rectangle {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.leftMargin: 1
            anchors.topMargin: 1
            anchors.rightMargin: 1
            height: root.height
            color: "white"
            radius: root.radius
            z: -1
        }
    }

    Text {
        id: emptyText
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: header.bottom
        visible: listView.count <= 0
        color: priv.textColor
        text: qsTr("(empty)")
        textFormat: Text.PlainText
        font.pointSize: 9
        font.family: "Helvetica"
        horizontalAlignment: Qt.AlignHCenter
        wrapMode: Text.WordWrap
        clip: true
        padding: 3
    }

    ListView {
        property real oldContentY
        readonly property int scrollBarWidth: 15
        property int itemHeight: 24 // NOTE: Hard-coded value

        id: listView
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: header.bottom
        anchors.bottom: footer.top
        anchors.topMargin: 1
        clip: true
        model: root.model ? root.model.listModel : null
        boundsBehavior: Flickable.StopAtBounds
        onContentXChanged: updateVisibleIndexRange()
        onContentYChanged: updateVisibleIndexRange()
        onCountChanged: updateVisibleIndexRange()

        function getVisibleIndexRange() {
            if(count === 0)
                return [0, 0];

            let y1 = listView.contentY - itemHeight;
            let y2 = listView.contentY + listView.height;
            return [Math.max(0, Math.ceil(y1 / itemHeight)),
                    Math.min(Math.floor(y2 / itemHeight), count - 1)];
        }

        function updateVisibleIndexRange() {
            let range = getVisibleIndexRange();
            root.model.minIndex = range[0];
            root.model.maxIndex = range[1];
        }

        ScrollBar.vertical: ScrollBar {
            id: scrollBar
            anchors.right: listView.right
            anchors.rightMargin: 2
            width: 13
            visible: scrollBar.size < 1
            policy: ScrollBar.AlwaysOn

            contentItem: Rectangle {
                color: scrollBar.pressed ? Qt.rgba(0.47, 0.47, 0.47, 1) : (hoverHandler.hovered ? Qt.rgba(0.66, 0.66, 0.66, 1) : Qt.rgba(0.76, 0.76, 0.76, 1))

                HoverHandler {
                    id: hoverHandler
                }
            }

            background: null // background is a separate component because contentItem width can't be changed
        }

        Rectangle {
            // Scroll bar background
            id: scrollBarBg
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.rightMargin: 1
            visible: scrollBar.visible
            width: listView.scrollBarWidth
            color: Qt.rgba(0.95, 0.95, 0.95, 1)
        }

        delegate: RowLayout {
            width: scrollBar.visible ? listView.width - listView.scrollBarWidth - 6 : listView.width - 6
            height: implicitHeight + 2
            spacing: 6

            Text {
                color: priv.textColor
                text: index + 1
                font.pointSize: 9
                font.bold: true
                font.family: "Helvetica"
                Layout.leftMargin: 6
            }

            Item {
                height: 22
                Layout.fillWidth: true

                TextEdit {
                    // TextEdit instead of Text for mouse selection
                    id: itemText
                    anchors.left: parent.left
                    anchors.leftMargin: 3
                    color: "white"
                    text: value
                    textFormat: TextEdit.PlainText
                    font.pointSize: 9
                    font.family: "Helvetica"
                    selectByMouse: true
                    padding: 2
                    Layout.rightMargin: 6
                }

                Rectangle {
                    anchors.fill: parent
                    color: root.model ? root.model.color : "green"
                    border.color: color.darker(1.2)
                    radius: root.radius
                    z: -1
                }
            }
        }
    }

    Text {
        id: footer
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        color: priv.textColor
        text: qsTr("length %1").arg(listView.count)
        textFormat: Text.PlainText
        font.pointSize: 9
        font.bold: true
        font.family: "Helvetica"
        horizontalAlignment: Qt.AlignHCenter
        wrapMode: Text.WordWrap
        clip: true
        padding: 3

        Rectangle {
            anchors.fill: parent
            anchors.leftMargin: 1
            anchors.bottomMargin: 1
            anchors.rightMargin: 1
            anchors.topMargin: -5
            color: "white"
            radius: root.radius
            z: -1
        }
    }

    Rectangle {
        // for header and footer borders
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: header.bottom
        anchors.bottom: footer.top
        color: "transparent"
        border.color: root.border.color
    }
}
