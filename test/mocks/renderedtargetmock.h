#pragma once

#include <irenderedtarget.h>
#include <qnanoquickitem.h>
#include <gmock/gmock.h>

using namespace scratchcppgui;

namespace scratchcppgui
{

class RenderedTargetMock : public IRenderedTarget
{
    public:
        MOCK_METHOD(void, loadProperties, (), (override));
        MOCK_METHOD(void, loadCostume, (libscratchcpp::Costume *), (override));
        MOCK_METHOD(void, updateProperties, (), (override));

        MOCK_METHOD(libscratchcpp::IEngine *, engine, (), (const, override));
        MOCK_METHOD(void, setEngine, (libscratchcpp::IEngine *), (override));

        MOCK_METHOD(StageModel *, stageModel, (), (const, override));
        MOCK_METHOD(void, setStageModel, (StageModel *), (override));

        MOCK_METHOD(SpriteModel *, spriteModel, (), (const, override));
        MOCK_METHOD(void, setSpriteModel, (SpriteModel *), (override));

        MOCK_METHOD(libscratchcpp::Target *, scratchTarget, (), (const, override));

        MOCK_METHOD(qreal, width, (), (const, override));
        MOCK_METHOD(void, setWidth, (qreal), (override));

        MOCK_METHOD(qreal, height, (), (const, override));
        MOCK_METHOD(void, setHeight, (qreal), (override));

        MOCK_METHOD(QPointF, mapFromScene, (const QPointF &), (const, override));

        MOCK_METHOD(QBuffer *, bitmapBuffer, (), (override));
        MOCK_METHOD(const QString &, bitmapUniqueKey, (), (const, override));

        MOCK_METHOD(void, lockCostume, (), (override));
        MOCK_METHOD(void, unlockCostume, (), (override));

        MOCK_METHOD(bool, mirrorHorizontally, (), (const, override));

        MOCK_METHOD(bool, isSvg, (), (const, override));
        MOCK_METHOD(void, paintSvg, (QNanoPainter *), (override));

        MOCK_METHOD(void, updateHullPoints, (QOpenGLFramebufferObject *), (override));
        MOCK_METHOD(const std::vector<QPointF> &, hullPoints, (), (const, override));

        MOCK_METHOD(bool, contains, (const QPointF &), (const, override));
        MOCK_METHOD(QNanoQuickItemPainter *, createItemPainter, (), (const, override));
        MOCK_METHOD(void, hoverEnterEvent, (QHoverEvent *), (override));
        MOCK_METHOD(void, hoverLeaveEvent, (QHoverEvent *), (override));
        MOCK_METHOD(void, hoverMoveEvent, (QHoverEvent *), (override));
        MOCK_METHOD(void, mouseMoveEvent, (QMouseEvent *), (override));
        MOCK_METHOD(void, mousePressEvent, (QMouseEvent *), (override));
        MOCK_METHOD(void, mouseReleaseEvent, (QMouseEvent *), (override));
};

} // namespace scratchcppgui
