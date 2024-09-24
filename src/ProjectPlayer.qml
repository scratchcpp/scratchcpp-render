// SPDX-License-Identifier: LGPL-3.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import ScratchCPP.Render

import "internal"

ProjectScene {
	readonly property string fileName: loader.fileName
    property int stageWidth: 480
    property int stageHeight: 360
    readonly property bool running: loader.running
    readonly property list<string> unsupportedBlocks: loader.unsupportedBlocks
    property alias fps: loader.fps
    property alias turboMode: loader.turboMode
	property alias cloneLimit: loader.cloneLimit
    property alias spriteFencing: loader.spriteFencing
    property alias mute: loader.mute
    property alias hqPen: projectPenLayer.hqPen
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
    onLoaded: priv.loaded = true
    onFailedToLoad: priv.loaded = false

	function load(fileName) {
        priv.loading = true;
		loader.fileName = fileName;
	}

    QtObject {
        id: priv
        property bool loading: false
        property bool loaded: false
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

        onQuestionAsked: (question)=> {
            questionLoader.active = true;
            questionLoader.item.clear();
            questionLoader.item.question = question;
        }

        onQuestionAborted: questionLoader.active = false
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
        color: priv.loading || !priv.loaded ? "transparent" : "white"
        clip: true

        Component {
            id: renderedTextBubble

            Loader {
                property QtObject model: null
                active: model ? model.bubbleText !== "" : false
                z: model ? model.bubbleLayer : 0

                sourceComponent: TextBubble {
                    type: model.bubbleType
                    text: model.bubbleText
                    target: model.renderedTarget
                    stageScale: root.stageScale
                    stageWidth: root.stageWidth
                    stageHeight: root.stageHeight
                    x: target.x
                    y: target.y
                }

                Component.onCompleted: model = modelData
            }
        }

        RenderedTarget {
            id: stageTarget
            engine: loader.engine
            stageModel: loader.stage
            mouseArea: sceneMouseArea
            stageScale: root.stageScale
            onStageModelChanged: stageModel.renderedTarget = this
        }

        Loader {
            readonly property alias modelData: stageTarget.stageModel
            active: model ? model.bubbleText !== "" : false
            sourceComponent: renderedTextBubble
        }

        PenLayer {
            id: projectPenLayer
            engine: loader.engine
            anchors.top: parent.top
            anchors.left: parent.left
            width: hqPen ? parent.width : stageWidth
            height: hqPen ? parent.height : stageHeight
            scale: hqPen ? 1 : stageScale
            transformOrigin: Item.TopLeft
            visible: !priv.loading
        }

        Component {
            id: renderedSprite

            Item {
                anchors.fill: parent
                z: targetItem.z

                RenderedTarget {
                    id: targetItem
                    mouseArea: sceneMouseArea
                    stageScale: root.stageScale
                    transform: Scale { xScale: targetItem.mirrorHorizontally ? -1 : 1 }
                    Component.onCompleted: {
                        engine = loader.engine;
                        spriteModel = modelData;
                        spriteModel.renderedTarget = this;
                        spriteModel.penLayer = projectPenLayer;
                    }
                }

                // Uncomment to display sprite bounding boxes (for debugging)
                /*Rectangle {
                    function translateX(x) {
                        // Translates Scratch X-coordinate to the scene coordinate system
                        return root.stageScale * (root.stageWidth / 2 + x)
                    }

                    function translateY(y) {
                        // Translates Scratch Y-coordinate to the scene coordinate system
                        return root.stageScale * (root.stageHeight / 2 - y)
                    }

                    id: boundRect
                    color: "transparent"
                    border.color: "red"
                    border.width: 3
                    visible: targetItem.visible

                    function updatePosition() {
                        let bounds = targetItem.getQmlBounds();
                        boundRect.x = translateX(bounds.left);
                        boundRect.y = translateY(bounds.top);
                        width = bounds.width * root.stageScale;
                        height = -bounds.height * root.stageScale;
                    }

                    Connections {
                        target: targetItem

                        function onXChanged() { boundRect.updatePosition() }
                        function onYChanged() { boundRect.updatePosition() }
                        function onRotationChanged() { boundRect.updatePosition() }
                        function onWidthChanged() { boundRect.updatePosition() }
                        function onHeightChanged() { boundRect.updatePosition() }
                        function onScaleChanged() { boundRect.updatePosition() }
                    }

                    Connections {
                        property Scale transform: Scale {}
                        target: transform

                        function onXScaleChanged() { boundRect.updatePosition() }

                        Component.onCompleted: transform = targetItem.transform[0]
                    }
                }*/
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

        Repeater {
            id: textBubbles
            model: loader.sprites
            delegate: renderedTextBubble
        }

        SceneMouseArea {
            id: sceneMouseArea
            anchors.fill: parent
            stage: stageTarget
            projectLoader: loader
            onMouseMoved: (x, y)=> root.handleMouseMove(x, y)
            onMousePressed: root.handleMousePress()
            onMouseReleased: root.handleMouseRelease()
            onMouseWheelUp: root.handleMouseWheelUp()
            onMouseWheelDown: root.handleMouseWheelDown()
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

    Loader {
        id: questionLoader
        anchors.left: contentRect.left
        anchors.right: contentRect.right
        anchors.bottom: contentRect.bottom
        anchors.margins: 9
        active: false

        sourceComponent: Question {
            onClosed: {
                loader.answerQuestion(answer);
                questionLoader.active = false;
            }

            Component.onCompleted: forceActiveFocus()
        }
    }
}
