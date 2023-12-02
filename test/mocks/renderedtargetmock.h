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

        MOCK_METHOD(double, costumeWidth, (), (const, override));
        MOCK_METHOD(void, setCostumeWidth, (double), (override));

        MOCK_METHOD(double, costumeHeight, (), (const, override));
        MOCK_METHOD(void, setCostumeHeight, (double), (override));

        MOCK_METHOD(unsigned char *, svgBitmap, (), (const, override));
        MOCK_METHOD(QBuffer *, bitmapBuffer, (), (override));
        MOCK_METHOD(const QString &, bitmapUniqueKey, (), (const, override));

        MOCK_METHOD(void, lockCostume, (), (override));
        MOCK_METHOD(void, unlockCostume, (), (override));

        MOCK_METHOD(bool, mirrorHorizontally, (), (const, override));

        MOCK_METHOD(QNanoQuickItemPainter *, createItemPainter, (), (const, override));
};

} // namespace scratchcppgui
