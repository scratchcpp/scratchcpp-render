// SPDX-License-Identifier: LGPL-3.0-or-later

import QtQuick
import ScratchCPPGui

ProjectScene {
    property string fileName
    property int stageWidth: 480
    property int stageHeight: 360

    id: root
    width: stageWidth
    height: stageHeight
    clip: true

    ProjectLoader {
        id: loader
        fileName: root.fileName
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
            Component.onCompleted: modelData.renderedTarget = this
        }
    }
}
