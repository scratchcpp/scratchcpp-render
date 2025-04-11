#include <QOffscreenSurface>
#include <QOpenGLExtraFunctions>
#include <QBuffer>
#include <QFile>
#include <QSignalSpy>
#include <penlayer.h>
#include <penattributes.h>
#include <projectloader.h>
#include <spritemodel.h>
#include <renderedtarget.h>
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
            emit m_context.aboutToBeDestroyed();
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
    QSignalSpy spy(&penLayer, &PenLayer::engineChanged);
    ASSERT_EQ(penLayer.engine(), nullptr);

    EngineMock engine1, engine2;
    penLayer.setWidth(480);
    penLayer.setHeight(360);
    EXPECT_CALL(engine1, stageWidth()).WillOnce(Return(480));
    penLayer.setEngine(&engine1);
    ASSERT_EQ(penLayer.engine(), &engine1);
    ASSERT_EQ(spy.count(), 1);

    penLayer.setWidth(500);
    penLayer.setHeight(400);
    EXPECT_CALL(engine2, stageWidth()).WillOnce(Return(500));
    penLayer.setEngine(&engine2);
    ASSERT_EQ(penLayer.engine(), &engine2);
    ASSERT_EQ(spy.count(), 2);

    penLayer.setEngine(nullptr);
    ASSERT_EQ(penLayer.engine(), nullptr);
    ASSERT_EQ(spy.count(), 3);
}

TEST_F(PenLayerTest, HqPen)
{
    PenLayer penLayer;
    QSignalSpy spy(&penLayer, &PenLayer::hqPenChanged);
    ASSERT_FALSE(penLayer.hqPen());

    penLayer.setHqPen(true);
    ASSERT_TRUE(penLayer.hqPen());
    ASSERT_EQ(spy.count(), 1);

    penLayer.setHqPen(false);
    ASSERT_FALSE(penLayer.hqPen());
    ASSERT_EQ(spy.count(), 2);
}

TEST_F(PenLayerTest, FramebufferObject)
{
    PenLayer penLayer;
    ASSERT_TRUE(penLayer.antialiasingEnabled());

    EngineMock engine1, engine2, engine3;
    penLayer.setWidth(480);
    penLayer.setHeight(360);
    EXPECT_CALL(engine1, stageWidth()).WillOnce(Return(480));
    penLayer.setEngine(&engine1);

    QOpenGLFramebufferObject *fbo = penLayer.framebufferObject();
    ASSERT_EQ(fbo->width(), 480);
    ASSERT_EQ(fbo->height(), 360);
    ASSERT_EQ(fbo->format().attachment(), QOpenGLFramebufferObject::CombinedDepthStencil);
    ASSERT_EQ(fbo->format().samples(), 0);

    penLayer.setAntialiasingEnabled(false);
    ASSERT_FALSE(penLayer.antialiasingEnabled());

    penLayer.setWidth(500);
    penLayer.setHeight(400);
    EXPECT_CALL(engine2, stageWidth()).WillOnce(Return(500));
    penLayer.setEngine(&engine2);

    fbo = penLayer.framebufferObject();
    ASSERT_EQ(fbo->width(), 500);
    ASSERT_EQ(fbo->height(), 400);
    ASSERT_EQ(fbo->format().attachment(), QOpenGLFramebufferObject::CombinedDepthStencil);
    ASSERT_EQ(fbo->format().samples(), 0);

    penLayer.setWidth(960);
    penLayer.setHeight(720);
    EXPECT_CALL(engine3, stageWidth()).WillOnce(Return(480));
    penLayer.setEngine(&engine3);

    fbo = penLayer.framebufferObject();
    ASSERT_EQ(fbo->width(), 960);
    ASSERT_EQ(fbo->height(), 720);
    ASSERT_EQ(fbo->format().attachment(), QOpenGLFramebufferObject::CombinedDepthStencil);
    ASSERT_EQ(fbo->format().samples(), 0);

    EXPECT_CALL(engine3, stageWidth()).Times(3).WillRepeatedly(Return(100));
    penLayer.setHqPen(true);
    penLayer.setWidth(500);
    penLayer.setHeight(400);
    penLayer.setEngine(&engine3);

    fbo = penLayer.framebufferObject();
    ASSERT_EQ(fbo->width(), 500);
    ASSERT_EQ(fbo->height(), 400);
    ASSERT_EQ(fbo->format().attachment(), QOpenGLFramebufferObject::CombinedDepthStencil);
    ASSERT_EQ(fbo->format().samples(), 0);
}

TEST_F(PenLayerTest, Refresh)
{
    PenLayer penLayer;

    EngineMock engine1, engine2, engine3;
    penLayer.setWidth(480);
    penLayer.setHeight(360);
    EXPECT_CALL(engine1, stageWidth()).WillOnce(Return(480));
    penLayer.setEngine(&engine1);

    penLayer.setWidth(500);
    penLayer.setHeight(400);

    EXPECT_CALL(engine1, stageWidth()).WillOnce(Return(500));
    penLayer.refresh();

    QOpenGLFramebufferObject *fbo = penLayer.framebufferObject();
    ASSERT_EQ(fbo->width(), 500);
    ASSERT_EQ(fbo->height(), 400);
}

TEST_F(PenLayerTest, GetProjectPenLayer)
{
    PenLayer penLayer;
    penLayer.setWidth(480);
    penLayer.setHeight(360);
    ASSERT_EQ(PenLayer::getProjectPenLayer(nullptr), nullptr);

    EngineMock engine1, engine2;
    ASSERT_EQ(PenLayer::getProjectPenLayer(&engine1), nullptr);
    ASSERT_EQ(PenLayer::getProjectPenLayer(&engine2), nullptr);

    EXPECT_CALL(engine1, stageWidth()).WillOnce(Return(1));
    penLayer.setEngine(&engine1);
    ASSERT_EQ(PenLayer::getProjectPenLayer(&engine1), &penLayer);
    ASSERT_EQ(PenLayer::getProjectPenLayer(&engine2), nullptr);

    EXPECT_CALL(engine2, stageWidth()).WillOnce(Return(1));
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
    penLayer.setWidth(480);
    penLayer.setHeight(360);
    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
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
    penLayer.setWidth(480);
    penLayer.setHeight(360);
    EngineMock engine;
    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    penLayer.setEngine(&engine);

    auto draw = [&penLayer]() {
        PenAttributes attr;
        attr.color = QNanoColor(255, 0, 0);
        attr.diameter = 3;

        penLayer.drawPoint(attr, 63, 164);
        penLayer.drawPoint(attr, -56, 93);
        penLayer.drawPoint(attr, 130, 77);

        attr.color = QNanoColor(0, 128, 0, 128);
        attr.diameter = 10;

        penLayer.drawPoint(attr, 152, -158);
        penLayer.drawPoint(attr, -228, 145);
        penLayer.drawPoint(attr, -100, 139);

        attr.color = QNanoColor(255, 50, 200, 185);
        attr.diameter = 25.6;

        penLayer.drawPoint(attr, -11, 179);
        penLayer.drawPoint(attr, 90, -48);
        penLayer.drawPoint(attr, -54, 21);
    };

    draw();

    {
        QOpenGLFramebufferObject *fbo = penLayer.framebufferObject();
        QImage image = fbo->toImage();
        QBuffer buffer;
        image.save(&buffer, "png");
        QFile ref("points.png");
        ref.open(QFile::ReadOnly);
        buffer.open(QFile::ReadOnly);
        ASSERT_EQ(ref.readAll(), buffer.readAll());
    }

    // Test HQ pen - resize existing texture
    EXPECT_CALL(engine, stageWidth()).Times(3).WillRepeatedly(Return(480));
    penLayer.setHqPen(true);
    penLayer.setWidth(720);
    penLayer.setHeight(540);

    {
        QOpenGLFramebufferObject *fbo = penLayer.framebufferObject();
        QImage image = fbo->toImage();
        QImage ref("points_resized.png");
        ASSERT_LE(fuzzyCompareImages(image, ref), 0.01);
    }

    // Test HQ pen - draw
    penLayer.clear();
    draw();

    {
        QOpenGLFramebufferObject *fbo = penLayer.framebufferObject();
        QImage image = fbo->toImage();
        QBuffer buffer;
        image.save(&buffer, "png");
        QFile ref("points_hq.png");
        ref.open(QFile::ReadOnly);
        buffer.open(QFile::ReadOnly);
        ASSERT_EQ(ref.readAll(), buffer.readAll());
    }
}

TEST_F(PenLayerTest, DrawLine)
{
    PenLayer penLayer;
    penLayer.setAntialiasingEnabled(false);
    penLayer.setWidth(480);
    penLayer.setHeight(360);
    EngineMock engine;
    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    penLayer.setEngine(&engine);

    auto draw = [&penLayer]() {
        PenAttributes attr;
        attr.color = QNanoColor(255, 0, 0);
        attr.diameter = 3;

        penLayer.drawLine(attr, 63, 164, -56, 93);
        penLayer.drawLine(attr, 130, 77, 125, -22);

        attr.color = QNanoColor(0, 128, 0, 128);
        attr.diameter = 225;

        penLayer.drawLine(attr, -225, 25, -175, -25);

        attr.diameter = 10;
        penLayer.drawLine(attr, -100, 139, 20, 72);

        attr.color = QNanoColor(255, 50, 200, 185);
        attr.diameter = 25.6;

        penLayer.drawLine(attr, -11, 179, 90, -48);
        penLayer.drawLine(attr, -54, 21, 88, -6);
    };

    draw();

    {
        QOpenGLFramebufferObject *fbo = penLayer.framebufferObject();
        QImage image = fbo->toImage().scaled(240, 180);
        QBuffer buffer;
        image.save(&buffer, "png");
        QFile ref("lines.png");
        ref.open(QFile::ReadOnly);
        buffer.open(QFile::ReadOnly);
        ASSERT_EQ(ref.readAll(), buffer.readAll());
    }

    // Test HQ pen
    penLayer.clear();
    EXPECT_CALL(engine, stageWidth()).Times(3).WillRepeatedly(Return(480));
    penLayer.setHqPen(true);
    penLayer.setWidth(720);
    penLayer.setHeight(540);
    draw();

    {
        QOpenGLFramebufferObject *fbo = penLayer.framebufferObject();
        QImage image = fbo->toImage();
        QBuffer buffer;
        image.save(&buffer, "png");
        QFile ref("lines_hq.png");
        ref.open(QFile::ReadOnly);
        buffer.open(QFile::ReadOnly);
        ASSERT_EQ(ref.readAll(), buffer.readAll());
    }
}

TEST_F(PenLayerTest, Stamp)
{
    static const std::chrono::milliseconds timeout(5000);
    auto startTime = std::chrono::steady_clock::now();

    PenLayer penLayer;
    penLayer.setWidth(480);
    penLayer.setHeight(360);
    EngineMock engine;
    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    penLayer.setEngine(&engine);

    ProjectLoader loader;
    loader.setFileName("stamp_env.sb3");

    while (loader.loadStatus() != ProjectLoader::LoadStatus::Loaded)
        ASSERT_LE(std::chrono::steady_clock::now(), startTime + timeout);

    EXPECT_CALL(engine, stageWidth()).WillRepeatedly(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillRepeatedly(Return(360));
    std::vector<std::unique_ptr<RenderedTarget>> targets;
    StageModel *stage = loader.stage();
    targets.push_back(std::make_unique<RenderedTarget>());
    targets.back()->setStageModel(stage);
    targets.back()->setEngine(loader.engine());
    targets.back()->loadCostumes();
    targets.back()->updateCostume(stage->stage()->currentCostume().get());
    targets.back()->setGraphicEffect(ShaderManager::Effect::Color, 25);
    stage->setRenderedTarget(targets.back().get());
    const auto &sprites = loader.spriteList();

    int i = 0;

    for (SpriteModel *sprite : sprites) {
        targets.push_back(std::make_unique<RenderedTarget>());
        targets.back()->setSpriteModel(sprite);
        targets.back()->setEngine(loader.engine());
        targets.back()->loadCostumes();
        targets.back()->updateCostume(sprite->sprite()->currentCostume().get());
        targets.back()->setGraphicEffect(ShaderManager::Effect::Color, i * 25);
        targets.back()->setGraphicEffect(ShaderManager::Effect::Ghost, i * 5);
        targets.back()->setGraphicEffect(ShaderManager::Effect::Pixelate, i * 25);
        sprite->setRenderedTarget(targets.back().get());
        i++;
    }

    for (const auto &target : targets)
        penLayer.stamp(target.get());

    {
        QOpenGLFramebufferObject *fbo = penLayer.framebufferObject();
        QImage image = fbo->toImage().scaled(240, 180);
        QImage ref("stamp.png");
        ASSERT_LE(fuzzyCompareImages(image, ref), 0.22);
    }

    // Test HQ pen
    penLayer.clear();
    penLayer.setHqPen(true);
    penLayer.setWidth(720);
    penLayer.setHeight(540);

    for (const auto &target : targets)
        penLayer.stamp(target.get());

    {
        QOpenGLFramebufferObject *fbo = penLayer.framebufferObject();
        QImage image = fbo->toImage();
        QImage ref("stamp_hq.png");
        ASSERT_LE(fuzzyCompareImages(image, ref), 0.42);
    }
}

TEST_F(PenLayerTest, TextureData)
{
    PenLayer penLayer;
    penLayer.setWidth(6);
    penLayer.setHeight(4);
    penLayer.setAntialiasingEnabled(false);
    EngineMock engine;
    EXPECT_CALL(engine, stageWidth()).WillRepeatedly(Return(6));
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

    // Test HQ pen
    penLayer.clear();
    EXPECT_CALL(engine, stageWidth()).Times(3).WillRepeatedly(Return(480));
    penLayer.setHqPen(true);
    penLayer.setWidth(720);
    penLayer.setHeight(540);

    attr = PenAttributes();
    attr.color = QNanoColor(255, 0, 0);
    attr.diameter = 1;
    penLayer.drawLine(attr, -3, 2, 3, -2);
    ASSERT_EQ(penLayer.colorAtScratchPoint(-3, 2), qRgb(255, 0, 0));
    ASSERT_EQ(penLayer.colorAtScratchPoint(0, 2), qRgba(0, 0, 0, 0));
    ASSERT_EQ(penLayer.colorAtScratchPoint(-1, 1), qRgb(255, 0, 0));

    bounds = penLayer.getBounds();
    ASSERT_EQ(std::round(bounds.left() * 100) / 100, -3.33);
    ASSERT_EQ(bounds.top(), 2);
    ASSERT_EQ(std::round(bounds.right() * 100) / 100, 3.67);
    ASSERT_EQ(bounds.bottom(), -3);

    attr.color = QNanoColor(0, 128, 0, 128);
    attr.diameter = 2;
    penLayer.drawLine(attr, -3, -2, 3, 2);
    ASSERT_EQ(penLayer.colorAtScratchPoint(-3, 2), qRgb(255, 0, 0));
    ASSERT_EQ(penLayer.colorAtScratchPoint(0, 2), 0);
    ASSERT_EQ(penLayer.colorAtScratchPoint(-1, 1), qRgb(255, 0, 0));

    bounds = penLayer.getBounds();
    ASSERT_EQ(std::round(bounds.left() * 100) / 100, -3.33);
    ASSERT_EQ(std::round(bounds.top() * 100) / 100, 2.67);
    ASSERT_EQ(std::round(bounds.right() * 100) / 100, 4.33);
    ASSERT_EQ(std::round(bounds.bottom() * 100) / 100, -3.67);

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
    ASSERT_EQ(std::round(bounds.top() * 100) / 100, 1.33);
    ASSERT_EQ(std::round(bounds.right() * 100) / 100, 1.67);
    ASSERT_EQ(std::round(bounds.bottom() * 100) / 100, -1.67);

    attr.diameter = 2;
    penLayer.drawPoint(attr, -2, 0);
    ASSERT_EQ(penLayer.colorAtScratchPoint(0, 1), qRgba(0, 0, 0, 0));
    ASSERT_EQ(penLayer.colorAtScratchPoint(0, 0), qRgb(0, 255, 0));
    ASSERT_EQ(penLayer.colorAtScratchPoint(-3, 1), 0);

    bounds = penLayer.getBounds();
    ASSERT_EQ(std::round(bounds.left() * 100) / 100, -2.67);
    ASSERT_EQ(std::round(bounds.top() * 100) / 100, 1.33);
    ASSERT_EQ(std::round(bounds.right() * 100) / 100, 1.67);
    ASSERT_EQ(std::round(bounds.bottom() * 100) / 100, -1.67);
}
