// SPDX-License-Identifier: LGPL-3.0-or-later

import QtQuick
import ScratchCPP.Render

TextBubbleShape {
    id: root
    property string text: ""
    property RenderedTarget target: null
    property double stageWidth: 0
    property double stageHeight: 0

    QtObject {
        // https://github.com/scratchfoundation/scratch-render/blob/ac935423afe3ba79235750eecb1e443474c6eb09/src/TextBubbleSkin.js#L7-L26
        id: priv
        readonly property int maxLineWidth: 170
        readonly property int minWidth: 50
        readonly property int padding: 10
        readonly property int tailHeight: 12

        readonly property string fontFamily: "Helvetica"
        readonly property int fontPixelSize: 14
        readonly property int lineHeight: 16

        readonly property color textFill: '#575E75'

        function translateX(x) {
            // Translates Scratch X-coordinate to the scene coordinate system
            return root.stageScale * (root.stageWidth / 2 + x)
        }

        function translateY(y) {
            // Translates Scratch Y-coordinate to the scene coordinate system
            return root.stageScale * (root.stageHeight / 2 - y)
        }
    }

    nativeWidth: Math.max(bubbleText.contentWidth, priv.minWidth) + 2 * priv.padding
    nativeHeight: bubbleText.height + 2 * priv.padding + priv.tailHeight

    function positionBubble() {
        // https://github.com/scratchfoundation/scratch-vm/blob/7313ce5199f8a3da7850085d0f7f6a3ca2c89bf6/src/blocks/scratch3_looks.js#L158
        if(!target.visible)
            return;

        const targetBounds = target.getBoundsForBubble();
        const stageBounds = Qt.rect(-root.stageWidth / 2, root.stageHeight / 2, root.stageWidth, -root.stageHeight);

        if (onSpriteRight && nativeWidth + targetBounds.right > stageBounds.right &&
            (targetBounds.left - nativeWidth > stageBounds.left)) { // Only flip if it would fit
            onSpriteRight = false;
        } else if (!onSpriteRight && targetBounds.left - nativeWidth < stageBounds.left &&
            (nativeWidth + targetBounds.right < stageBounds.right)) { // Only flip if it would fit
            onSpriteRight = true;
        }

        const pos = [
            onSpriteRight ? (
                Math.max(
                    stageBounds.left, // Bubble should not extend past left edge of stage
                    Math.min(stageBounds.right - nativeWidth, targetBounds.right)
                )
            ) : (
                Math.min(
                    stageBounds.right - nativeWidth, // Bubble should not extend past right edge of stage
                    Math.max(stageBounds.left, targetBounds.left - nativeWidth)
                )
            ),
            // Bubble should not extend past the top of the stage
            Math.min(stageBounds.top, targetBounds.bottom + nativeHeight)
        ];

        x = priv.translateX(pos[0]);
        y = priv.translateY(pos[1]);
    }

    Connections {
        target: root.target

        function onXChanged() { positionBubble() }
        function onYChanged() { positionBubble() }
        function onRotationChanged() { positionBubble() }
        function onWidthChanged() { positionBubble() }
        function onHeightChanged() { positionBubble() }
        function onScaleChanged() { positionBubble() }
    }

    Connections {
        property Scale transform: Scale {}
        target: transform

        function onXScaleChanged() { positionBubble() }

        Component.onCompleted: transform = root.target.transform[0]
    }

    Text {
        id: bubbleText
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.margins: priv.padding * root.stageScale
        width: priv.maxLineWidth
        scale: root.stageScale
        transformOrigin: Item.TopLeft
        text: root.text
        wrapMode: Text.Wrap
        color: priv.textFill
        lineHeight: priv.lineHeight
        lineHeightMode: Text.FixedHeight
        font.family: priv.fontFamily
        font.pixelSize: priv.fontPixelSize
    }

    Component.onCompleted: positionBubble()
}
