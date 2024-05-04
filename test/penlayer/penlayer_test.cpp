#include <QOffscreenSurface>
#include <QOpenGLExtraFunctions>
#include <QBuffer>
#include <QFile>
#include <penlayer.h>
#include <penattributes.h>
#include <qnanopainter.h>
#include <enginemock.h>

#include "../common.h"

using namespace scratchcpprender;
using namespace libscratchcpp;

using ::testing::Return;

class PenLayerTest : public testing::Test
{
    public:
        void SetUp() override
        {
            m_context.create();
            ASSERT_TRUE(m_context.isValid());

            m_surface.setFormat(m_context.format());
            m_surface.create();
            Q_ASSERT(m_surface.isValid());
            m_context.makeCurrent(&m_surface);
        }

        void TearDown() override
        {
            ASSERT_EQ(m_context.surface(), &m_surface);
            m_context.doneCurrent();
        }

        QOpenGLContext m_context;
        QOffscreenSurface m_surface;
};

TEST_F(PenLayerTest, Constructors)
{
    PenLayer penLayer1;
    PenLayer penLayer2(&penLayer1);
    ASSERT_EQ(penLayer2.parent(), &penLayer1);
    ASSERT_EQ(penLayer2.parentItem(), &penLayer1);
    ASSERT_FALSE(penLayer1.smooth());
}

TEST_F(PenLayerTest, Engine)
{
    PenLayer penLayer;
    ASSERT_EQ(penLayer.engine(), nullptr);

    EngineMock engine1, engine2;
    EXPECT_CALL(engine1, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine1, stageHeight()).WillOnce(Return(360));
    penLayer.setEngine(&engine1);
    ASSERT_EQ(penLayer.engine(), &engine1);

    EXPECT_CALL(engine2, stageWidth()).WillOnce(Return(500));
    EXPECT_CALL(engine2, stageHeight()).WillOnce(Return(400));
    penLayer.setEngine(&engine2);
    ASSERT_EQ(penLayer.engine(), &engine2);

    penLayer.setEngine(nullptr);
    ASSERT_EQ(penLayer.engine(), nullptr);
}

TEST_F(PenLayerTest, FramebufferObject)
{
    PenLayer penLayer;
    ASSERT_TRUE(penLayer.antialiasingEnabled());

    EngineMock engine1, engine2;
    EXPECT_CALL(engine1, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine1, stageHeight()).WillOnce(Return(360));
    penLayer.setEngine(&engine1);

    QOpenGLFramebufferObject *fbo = penLayer.framebufferObject();
    ASSERT_EQ(fbo->width(), 480);
    ASSERT_EQ(fbo->height(), 360);
    ASSERT_EQ(fbo->format().attachment(), QOpenGLFramebufferObject::CombinedDepthStencil);
    ASSERT_EQ(fbo->format().samples(), 0);

    penLayer.setAntialiasingEnabled(false);
    ASSERT_FALSE(penLayer.antialiasingEnabled());

    EXPECT_CALL(engine2, stageWidth()).WillOnce(Return(500));
    EXPECT_CALL(engine2, stageHeight()).WillOnce(Return(400));
    penLayer.setEngine(&engine2);

    fbo = penLayer.framebufferObject();
    ASSERT_EQ(fbo->width(), 500);
    ASSERT_EQ(fbo->height(), 400);
    ASSERT_EQ(fbo->format().attachment(), QOpenGLFramebufferObject::CombinedDepthStencil);
    ASSERT_EQ(fbo->format().samples(), 0);
}

TEST_F(PenLayerTest, GetProjectPenLayer)
{
    PenLayer penLayer;
    ASSERT_EQ(PenLayer::getProjectPenLayer(nullptr), nullptr);

    EngineMock engine1, engine2;
    ASSERT_EQ(PenLayer::getProjectPenLayer(&engine1), nullptr);
    ASSERT_EQ(PenLayer::getProjectPenLayer(&engine2), nullptr);

    EXPECT_CALL(engine1, stageWidth()).WillOnce(Return(1));
    EXPECT_CALL(engine1, stageHeight()).WillOnce(Return(1));
    penLayer.setEngine(&engine1);
    ASSERT_EQ(PenLayer::getProjectPenLayer(&engine1), &penLayer);
    ASSERT_EQ(PenLayer::getProjectPenLayer(&engine2), nullptr);

    EXPECT_CALL(engine2, stageWidth()).WillOnce(Return(1));
    EXPECT_CALL(engine2, stageHeight()).WillOnce(Return(1));
    penLayer.setEngine(&engine2);
    ASSERT_EQ(PenLayer::getProjectPenLayer(&engine1), nullptr);
    ASSERT_EQ(PenLayer::getProjectPenLayer(&engine2), &penLayer);

    penLayer.setEngine(nullptr);
    ASSERT_EQ(PenLayer::getProjectPenLayer(&engine1), nullptr);
    ASSERT_EQ(PenLayer::getProjectPenLayer(&engine2), nullptr);

    PenLayer::addPenLayer(&engine1, &penLayer);
    ASSERT_EQ(PenLayer::getProjectPenLayer(&engine1), &penLayer);
    ASSERT_EQ(PenLayer::getProjectPenLayer(&engine2), nullptr);
}

TEST_F(PenLayerTest, Clear)
{
    PenLayer penLayer;
    EngineMock engine;
    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    penLayer.setEngine(&engine);

    QOpenGLExtraFunctions glF(&m_context);
    glF.initializeOpenGLFunctions();

    QOpenGLFramebufferObject *fbo = penLayer.framebufferObject();
    QImage image1 = fbo->toImage();

    // The initial texture must contain only fully transparent pixels
    for (int y = 0; y < image1.height(); y++) {
        for (int x = 0; x < image1.width(); x++)
            ASSERT_EQ(QColor::fromRgba(image1.pixel(x, y)).alphaF(), 0);
    }

    // Paint something first to test clear()
    fbo->bind();
    QNanoPainter painter;
    painter.beginFrame(fbo->width(), fbo->height());
    painter.setStrokeStyle(QNanoColor(0, 0, 0));
    painter.moveTo(0, 0);
    painter.lineTo(fbo->width(), fbo->height());
    painter.stroke();
    painter.endFrame();
    fbo->release();

    penLayer.clear();
    QImage image2 = fbo->toImage();

    // The image must contain only fully transparent pixels
    for (int y = 0; y < image2.height(); y++) {
        for (int x = 0; x < image2.width(); x++)
            ASSERT_EQ(QColor::fromRgba(image2.pixel(x, y)).alphaF(), 0);
    }
}

TEST_F(PenLayerTest, DrawPoint)
{
    PenLayer penLayer;
    penLayer.setAntialiasingEnabled(false);
    EngineMock engine;
    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    penLayer.setEngine(&engine);

    PenAttributes attr;
    attr.color = QNanoColor(255, 0, 0);
    attr.diameter = 3;

    EXPECT_CALL(engine, stageWidth()).Times(3).WillRepeatedly(Return(480));
    EXPECT_CALL(engine, stageHeight()).Times(3).WillRepeatedly(Return(360));
    penLayer.drawPoint(attr, 63, 164);
    penLayer.drawPoint(attr, -56, 93);
    penLayer.drawPoint(attr, 130, 77);

    attr.color = QNanoColor(0, 128, 0, 128);
    attr.diameter = 10;

    EXPECT_CALL(engine, stageWidth()).Times(3).WillRepeatedly(Return(480));
    EXPECT_CALL(engine, stageHeight()).Times(3).WillRepeatedly(Return(360));
    penLayer.drawPoint(attr, 152, -158);
    penLayer.drawPoint(attr, -228, 145);
    penLayer.drawPoint(attr, -100, 139);

    attr.color = QNanoColor(255, 50, 200, 185);
    attr.diameter = 25.6;

    EXPECT_CALL(engine, stageWidth()).Times(3).WillRepeatedly(Return(480));
    EXPECT_CALL(engine, stageHeight()).Times(3).WillRepeatedly(Return(360));
    penLayer.drawPoint(attr, -11, 179);
    penLayer.drawPoint(attr, 90, -48);
    penLayer.drawPoint(attr, -54, 21);

    QOpenGLFramebufferObject *fbo = penLayer.framebufferObject();
    QImage image = fbo->toImage();
    QBuffer buffer;
    image.save(&buffer, "png");
    QFile ref("points.png");
    ref.open(QFile::ReadOnly);
    buffer.open(QFile::ReadOnly);
    ASSERT_EQ(ref.readAll(), buffer.readAll());
}

TEST_F(PenLayerTest, DrawLine)
{
    PenLayer penLayer;
    penLayer.setAntialiasingEnabled(false);
    EngineMock engine;
    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    penLayer.setEngine(&engine);

    PenAttributes attr;
    attr.color = QNanoColor(255, 0, 0);
    attr.diameter = 3;

    EXPECT_CALL(engine, stageWidth()).Times(2).WillRepeatedly(Return(480));
    EXPECT_CALL(engine, stageHeight()).Times(2).WillRepeatedly(Return(360));
    penLayer.drawLine(attr, 63, 164, -56, 93);
    penLayer.drawLine(attr, 130, 77, 125, -22);

    attr.color = QNanoColor(0, 128, 0, 128);
    attr.diameter = 10;

    EXPECT_CALL(engine, stageWidth()).Times(2).WillRepeatedly(Return(480));
    EXPECT_CALL(engine, stageHeight()).Times(2).WillRepeatedly(Return(360));
    penLayer.drawLine(attr, 152, -158, -228, 145);
    penLayer.drawLine(attr, -100, 139, 20, 72);

    attr.color = QNanoColor(255, 50, 200, 185);
    attr.diameter = 25.6;

    EXPECT_CALL(engine, stageWidth()).Times(2).WillRepeatedly(Return(480));
    EXPECT_CALL(engine, stageHeight()).Times(2).WillRepeatedly(Return(360));
    penLayer.drawLine(attr, -11, 179, 90, -48);
    penLayer.drawLine(attr, -54, 21, 88, -6);

    QOpenGLFramebufferObject *fbo = penLayer.framebufferObject();
    QImage image = fbo->toImage().scaled(240, 180);
    QBuffer buffer;
    image.save(&buffer, "png");
    QFile ref("lines.png");
    ref.open(QFile::ReadOnly);
    buffer.open(QFile::ReadOnly);
    ASSERT_EQ(ref.readAll(), buffer.readAll());
}

TEST_F(PenLayerTest, TextureData)
{
    PenLayer penLayer;
    penLayer.setAntialiasingEnabled(false);
    EngineMock engine;
    EXPECT_CALL(engine, stageWidth()).WillRepeatedly(Return(6));
    EXPECT_CALL(engine, stageHeight()).WillRepeatedly(Return(4));
    penLayer.setEngine(&engine);

    PenAttributes attr;
    attr.color = QNanoColor(255, 0, 0);
    attr.diameter = 1;
    penLayer.drawLine(attr, -3, 2, 3, -2);
    ASSERT_EQ(penLayer.colorAtScratchPoint(-3, 2), qRgb(255, 0, 0));
    ASSERT_EQ(penLayer.colorAtScratchPoint(0, 2), qRgba(0, 0, 0, 0));
    ASSERT_EQ(penLayer.colorAtScratchPoint(-1, 1), qRgb(255, 0, 0));

    Rect bounds = penLayer.getBounds();
    ASSERT_EQ(bounds.left(), -3);
    ASSERT_EQ(bounds.top(), 2);
    ASSERT_EQ(bounds.right(), 3);
    ASSERT_EQ(bounds.bottom(), -2);

    attr.color = QNanoColor(0, 128, 0, 128);
    attr.diameter = 2;
    penLayer.drawLine(attr, -3, -2, 3, 2);
    ASSERT_EQ(penLayer.colorAtScratchPoint(-3, 2), qRgb(255, 0, 0));
    ASSERT_EQ(penLayer.colorAtScratchPoint(0, 2), qRgba(0, 64, 0, 128));
    ASSERT_EQ(penLayer.colorAtScratchPoint(-1, 1), qRgb(127, 64, 0));

    bounds = penLayer.getBounds();
    ASSERT_EQ(bounds.left(), -3);
    ASSERT_EQ(bounds.top(), 2);
    ASSERT_EQ(bounds.right(), 3);
    ASSERT_EQ(bounds.bottom(), -2);

    penLayer.clear();
    ASSERT_EQ(penLayer.colorAtScratchPoint(-3, 2), 0);
    ASSERT_EQ(penLayer.colorAtScratchPoint(0, 2), 0);
    ASSERT_EQ(penLayer.colorAtScratchPoint(-1, 1), 0);

    bounds = penLayer.getBounds();
    ASSERT_EQ(bounds.left(), 0);
    ASSERT_EQ(bounds.top(), 0);
    ASSERT_EQ(bounds.right(), 0);
    ASSERT_EQ(bounds.bottom(), 0);

    attr.color = QNanoColor(0, 255, 0, 255);
    attr.diameter = 1;
    penLayer.drawLine(attr, 0, -1, 1, 1);
    ASSERT_EQ(penLayer.colorAtScratchPoint(0, 1), qRgba(0, 0, 0, 0));
    ASSERT_EQ(penLayer.colorAtScratchPoint(0, 0), qRgb(0, 255, 0));
    ASSERT_EQ(penLayer.colorAtScratchPoint(-3, 1), qRgba(0, 0, 0, 0));

    bounds = penLayer.getBounds();
    ASSERT_EQ(bounds.left(), 0);
    ASSERT_EQ(bounds.top(), 0);
    ASSERT_EQ(bounds.right(), 2);
    ASSERT_EQ(bounds.bottom(), -2);

    attr.diameter = 2;
    penLayer.drawPoint(attr, -2, 0);
    ASSERT_EQ(penLayer.colorAtScratchPoint(0, 1), qRgba(0, 0, 0, 0));
    ASSERT_EQ(penLayer.colorAtScratchPoint(0, 0), qRgb(0, 255, 0));
    ASSERT_EQ(penLayer.colorAtScratchPoint(-3, 1), qRgb(0, 255, 0));

    bounds = penLayer.getBounds();
    ASSERT_EQ(bounds.left(), -3);
    ASSERT_EQ(bounds.top(), 1);
    ASSERT_EQ(bounds.right(), 2);
    ASSERT_EQ(bounds.bottom(), -2);
}
