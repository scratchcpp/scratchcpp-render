// SPDX-License-Identifier: LGPL-3.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import ScratchCPP.Render

import "internal"

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

        onCloneCreated: (cloneModel)=> clones.model.append({"spriteModel": cloneModel})

        onCloneDeleted: (cloneModel)=> {
            // TODO: Removing the clone from C++ would probably be faster
            let i;

            for(i = 0; i < clones.model.count; i++) {
                if(clones.model.get(i).spriteModel === cloneModel)
                    break;
            }

            if(i === clones.model.count)
                console.error("error: deleted clone doesn't exist");
            else
                clones.model.remove(i);
        }

        onMonitorAdded: (monitorModel)=> monitors.model.append({"monitorModel": monitorModel})

        onMonitorRemoved: (monitorModel)=> {
            // TODO: Removing the monitor from C++ would probably be faster
            let i;

            for(i = 0; i < monitors.model.count; i++) {
                if(monitors.model.get(i).monitorModel === monitorModel)
                    break;
            }

            if(i !== monitors.model.count)
                monitors.model.remove(i);
        }
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

        PenLayer {
            id: projectPenLayer
            engine: loader.engine
            anchors.fill: parent
        }

        Component {
            id: renderedSprite

            RenderedTarget {
                id: target
                mouseArea: sceneMouseArea
                stageScale: root.stageScale
                transform: Scale { xScale: mirrorHorizontally ? -1 : 1 }
                Component.onCompleted: {
                    engine = loader.engine;
                    spriteModel = modelData;
                    spriteModel.renderedTarget = this;
                    spriteModel.penLayer = projectPenLayer;
                }
            }
        }

        Repeater {
            id: sprites
            model: loader.sprites
            delegate: renderedSprite
        }

        Repeater {
            id: clones
            model: ListModel {}
            delegate: renderedSprite
        }

        SceneMouseArea {
            id: sceneMouseArea
            anchors.fill: parent
            stage: stageTarget
            projectLoader: loader
            onMouseMoved: (x, y)=> root.handleMouseMove(x, y)
            onMousePressed: root.handleMousePress()
            onMouseReleased: root.handleMouseRelease()
        }

        Component {
            id: renderedValueMonitor

            ValueMonitor {
                model: parent.model
                scale: root.stageScale
                transformOrigin: Item.TopLeft
                x: model.x * scale
                y: model.y * scale
            }
        }

        Component {
            id: renderedListMonitor

            ListMonitor {
                model: parent.model
                scale: root.stageScale
                transformOrigin: Item.TopLeft
                x: model.x * scale
                y: model.y * scale
            }
        }

        Component {
            id: renderedMonitor

            Loader {
                readonly property MonitorModel model: monitorModel
                sourceComponent: monitorModel ? (monitorModel.type === MonitorModel.Value ? renderedValueMonitor : renderedListMonitor) : null
                active: sourceComponent != null
                z: loader.sprites.length + loader.clones.length + 1 // above all sprites
            }
        }

        Repeater {
            id: monitors
            model: ListModel {}
            delegate: renderedMonitor
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
    }
}
