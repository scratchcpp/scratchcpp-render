// SPDX-License-Identifier: LGPL-3.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import ScratchCPPGui

ProjectScene {
    property string fileName
    property alias fps: loader.fps
    property alias turboMode: loader.turboMode
	property alias cloneLimit: loader.cloneLimit
	property alias spriteFencing: loader.spriteFencing
    property bool showLoadingProgress: true
    readonly property bool loading: priv.loading
    readonly property int downloadedAssets: loader.downloadedAssets
    readonly property int assetCount: loader.assetCount
    signal loaded()
    signal failedToLoad()

    id: root
	clip: true
    engine: loader.engine
    onFileNameChanged: priv.loading = true;

    QtObject {
        id: priv
        property bool loading: false
    }

    ProjectLoader {
        id: loader
        fileName: root.fileName
		stageWidth: parent.width
		stageHeight: parent.height
        onLoadingFinished: {
            priv.loading = false;

            if(loadStatus)
                loaded();
            else
                failedToLoad();
        }
    }

    RenderedTarget {
        id: stageTarget
        engine: loader.engine
        stageModel: loader.stage
        onStageModelChanged: stageModel.renderedTarget = this
    }

    function start() {
        loader.start();
    }

    function stop() {
        loader.stop();
    }

    Repeater {
        id: sprites
        model: loader.sprites

        RenderedTarget {
            id: target
            engine: loader.engine
            spriteModel: modelData
            transform: Scale { xScale: mirrorHorizontally ? -1 : 1 }
            Component.onCompleted: modelData.renderedTarget = this
        }
    }

    Loader {
        anchors.fill: parent
        active: showLoadingProgress && loading

        sourceComponent: ColumnLayout {
            anchors.fill: parent

            Item { Layout.fillHeight: true }

            BusyIndicator {
                Layout.fillWidth: true
                Layout.maximumWidth: 100
                Layout.alignment: Qt.AlignHCenter
                running: true
            }

            Label {
                Layout.alignment: Qt.AlignHCenter
                font.bold: true
                font.pointSize: 12
                text: {
                    if(loading)
                        return assetCount == downloadedAssets ? qsTr("Loading project...") : qsTr("Downloading assets... (%1 of %2)").arg(downloadedAssets).arg(assetCount);
                    else
                        return "";
                }
            }

            ProgressBar {
                Layout.fillWidth: true
                from: 0
                to: assetCount
                value: downloadedAssets
                indeterminate: assetCount == downloadedAssets
            }

            Item { Layout.fillHeight: true }
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onPositionChanged: root.handleMouseMove(mouseX, mouseY)
        onPressed: root.handleMousePress()
        onReleased: root.handleMouseRelease()
    }
}
