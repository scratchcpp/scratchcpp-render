#pragma once

#include <ipenlayer.h>
#include <qnanoquickitem.h>
#include <gmock/gmock.h>

using namespace scratchcpprender;

namespace scratchcpprender
{

class PenLayerMock : public IPenLayer
{
    public:
        MOCK_METHOD(bool, antialiasingEnabled, (), (const, override));
        MOCK_METHOD(void, setAntialiasingEnabled, (bool), (override));

        MOCK_METHOD(libscratchcpp::IEngine *, engine, (), (const, override));
        MOCK_METHOD(void, setEngine, (libscratchcpp::IEngine *), (override));

        MOCK_METHOD(void, clear, (), (override));
        MOCK_METHOD(void, drawPoint, (const PenAttributes &, double, double), (override));
        MOCK_METHOD(void, drawLine, (const PenAttributes &, double, double, double, double), (override));

        MOCK_METHOD(QOpenGLFramebufferObject *, framebufferObject, (), (const, override));

        MOCK_METHOD(QNanoQuickItemPainter *, createItemPainter, (), (const, override));
};

} // namespace scratchcpprender
