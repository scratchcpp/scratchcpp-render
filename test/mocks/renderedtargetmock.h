#pragma once

#include <irenderedtarget.h>
#include <texture.h>
#include <qnanoquickitem.h>
#include <scratchcpp/rect.h>
#include <gmock/gmock.h>

using namespace scratchcpprender;

namespace scratchcpprender
{

class RenderedTargetMock : public IRenderedTarget
{
    public:
        MOCK_METHOD(void, updateVisibility, (bool), (override));
        MOCK_METHOD(void, updateX, (double), (override));
        MOCK_METHOD(void, updateY, (double), (override));
        MOCK_METHOD(void, updateSize, (double), (override));
        MOCK_METHOD(void, updateDirection, (double), (override));
        MOCK_METHOD(void, updateRotationStyle, (libscratchcpp::Sprite::RotationStyle), (override));
        MOCK_METHOD(void, updateLayerOrder, (int), (override));
        MOCK_METHOD(void, updateCostume, (libscratchcpp::Costume *), (override));

        MOCK_METHOD(bool, costumesLoaded, (), (const, override));
        MOCK_METHOD(void, loadCostumes, (), (override));

        MOCK_METHOD(void, beforeRedraw, (), (override));

        MOCK_METHOD(void, deinitClone, (), (override));

        MOCK_METHOD(libscratchcpp::IEngine *, engine, (), (const, override));
        MOCK_METHOD(void, setEngine, (libscratchcpp::IEngine *), (override));

        MOCK_METHOD(StageModel *, stageModel, (), (const, override));
        MOCK_METHOD(void, setStageModel, (StageModel *), (override));

        MOCK_METHOD(SpriteModel *, spriteModel, (), (const, override));
        MOCK_METHOD(void, setSpriteModel, (SpriteModel *), (override));

        MOCK_METHOD(libscratchcpp::Target *, scratchTarget, (), (const, override));

        MOCK_METHOD(SceneMouseArea *, mouseArea, (), (const, override));
        MOCK_METHOD(void, setMouseArea, (SceneMouseArea *), (override));

        MOCK_METHOD(double, stageScale, (), (const, override));
        MOCK_METHOD(void, setStageScale, (double), (override));

        MOCK_METHOD(qreal, width, (), (const, override));
        MOCK_METHOD(void, setWidth, (qreal), (override));

        MOCK_METHOD(qreal, height, (), (const, override));
        MOCK_METHOD(void, setHeight, (qreal), (override));

        MOCK_METHOD(QPointF, mapFromScene, (const QPointF &), (const, override));

        MOCK_METHOD(libscratchcpp::Rect, getBounds, (), (const, override));
        MOCK_METHOD(libscratchcpp::Rect, getFastBounds, (), (const, override));
        MOCK_METHOD(QRectF, getBoundsForBubble, (), (const, override));

        MOCK_METHOD(bool, mirrorHorizontally, (), (const, override));

        MOCK_METHOD(Texture, texture, (), (const, override));

        MOCK_METHOD((const std::unordered_map<ShaderManager::Effect, double> &), graphicEffects, (), (const, override));
        MOCK_METHOD(void, setGraphicEffect, (ShaderManager::Effect effect, double value), (override));
        MOCK_METHOD(void, clearGraphicEffects, (), (override));

        MOCK_METHOD(const std::vector<QPoint> &, hullPoints, (), (const, override));

        MOCK_METHOD(bool, contains, (const QPointF &), (const, override));
        MOCK_METHOD(bool, containsScratchPoint, (double, double), (const, override));
        MOCK_METHOD(QRgb, colorAtScratchPoint, (double, double), (const, override));

        MOCK_METHOD(bool, touchingClones, (const std::vector<libscratchcpp::Sprite *> &), (const, override));

        MOCK_METHOD(QNanoQuickItemPainter *, createItemPainter, (), (const, override));
        MOCK_METHOD(void, hoverEnterEvent, (QHoverEvent *), (override));
        MOCK_METHOD(void, hoverLeaveEvent, (QHoverEvent *), (override));
        MOCK_METHOD(void, hoverMoveEvent, (QHoverEvent *), (override));
        MOCK_METHOD(void, mouseMoveEvent, (QMouseEvent *), (override));
        MOCK_METHOD(void, mousePressEvent, (QMouseEvent *), (override));
        MOCK_METHOD(void, mouseReleaseEvent, (QMouseEvent *), (override));
};

} // namespace scratchcpprender
