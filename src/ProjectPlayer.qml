// SPDX-License-Identifier: LGPL-3.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import ScratchCPP.Render

ProjectScene {
    property string fileName
    property int stageWidth: 480
    property int stageHeight: 360
    property alias fps: loader.fps
    property alias turboMode: loader.turboMode
	property alias cloneLimit: loader.cloneLimit
	property alias spriteFencing: loader.spriteFencing
    property bool showLoadingProgress: true
    readonly property bool loading: priv.loading
    readonly property int downloadedAssets: loader.downloadedAssets
    readonly property int assetCount: loader.assetCount
    readonly property Rectangle stageRect: contentRect
    signal loaded()
    signal failedToLoad()

    id: root
    engine: loader.engine
    stageScale: (stageWidth == 0 || stageHeight == 0) ? 1 : Math.min(width / stageWidth, height / stageHeight)
    onFileNameChanged: priv.loading = true;

    QtObject {
        id: priv
        property bool loading: false
    }

    ProjectLoader {
        id: loader
        fileName: root.fileName
        stageWidth: root.stageWidth
        stageHeight: root.stageHeight
        onLoadingFinished: {
            priv.loading = false;

            if(loadStatus)
                loaded();
            else
                failedToLoad();
        }
        onStageChanged: stage.loadCostume();
    }

    function start() {
        loader.start();
    }

    function stop() {
        loader.stop();
    }

    Rectangle {
        id: contentRect
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        width: stageWidth * stageScale
        height: stageHeight * stageScale
        color: priv.loading ? "transparent" : "white"
        clip: true

        RenderedTarget {
            id: stageTarget
            engine: loader.engine
            stageModel: loader.stage
            mouseArea: sceneMouseArea
            stageScale: root.stageScale
            onStageModelChanged: stageModel.renderedTarget = this
        }

        Component {
            id: renderedSprite

            RenderedTarget {
                id: target
                engine: loader.engine
                spriteModel: modelData
                mouseArea: sceneMouseArea
                stageScale: root.stageScale
                transform: Scale { xScale: mirrorHorizontally ? -1 : 1 }
                Component.onCompleted: modelData.renderedTarget = this
            }
        }

        Repeater {
            id: sprites
            model: loader.sprites
            delegate: renderedSprite
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

        SceneMouseArea {
            id: sceneMouseArea
            anchors.fill: parent
            stage: stageTarget
            spriteRepeater: sprites
            onMouseMoved: (x, y)=> root.handleMouseMove(x, y)
            onMousePressed: root.handleMousePress()
            onMouseReleased: root.handleMouseRelease()
        }
    }
}
