// SPDX-License-Identifier: LGPL-3.0-or-later

import QtQuick
import ScratchCPPGui

ProjectScene {
    property string fileName
    property alias fps: loader.fps
    property alias turboMode: loader.turboMode
	property alias cloneLimit: loader.cloneLimit

    id: root
	clip: true

    ProjectLoader {
        id: loader
        fileName: root.fileName
		stageWidth: parent.width
		stageHeight: parent.height
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
}
