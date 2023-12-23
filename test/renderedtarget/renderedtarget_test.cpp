#include <QtTest/QSignalSpy>
#include <QOpenGLContext>
#include <QOffscreenSurface>
#include <qnanopainter.h>
#include <renderedtarget.h>
#include <stagemodel.h>
#include <spritemodel.h>
#include <scratchcpp/stage.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/costume.h>
#include <enginemock.h>

#include "../common.h"

using namespace scratchcppgui;
using namespace libscratchcpp;

using ::testing::Return;

class RenderedTargetTest : public testing::Test
{
    public:
        void createContextAndSurface(QOpenGLContext *context, QOffscreenSurface *surface)
        {
            QSurfaceFormat surfaceFormat;
            surfaceFormat.setMajorVersion(4);
            surfaceFormat.setMinorVersion(3);

            context->setFormat(surfaceFormat);
            context->create();
            ASSERT_TRUE(context->isValid());

            surface->setFormat(surfaceFormat);
            surface->create();
            ASSERT_TRUE(surface->isValid());

            context->makeCurrent(surface);
            ASSERT_EQ(QOpenGLContext::currentContext(), context);
        }
};

TEST_F(RenderedTargetTest, Constructors)
{
    RenderedTarget target1;
    RenderedTarget target2(&target1);
    ASSERT_EQ(target2.parent(), &target1);
    ASSERT_EQ(target2.parentItem(), &target1);
}

TEST_F(RenderedTargetTest, LoadAndUpdateProperties)
{
    RenderedTarget target;
    QSignalSpy mirrorHorizontallySpy(&target, &RenderedTarget::mirrorHorizontallyChanged);

    // Stage
    Stage stage;
    StageModel stageModel;
    stage.setInterface(&stageModel);
    target.setStageModel(&stageModel);
    Costume costume("", "", "");
    std::string costumeData = readFileStr("image.png");
    costume.setData(costumeData.size(), static_cast<void *>(costumeData.data()));
    costume.setRotationCenterX(-23);
    costume.setRotationCenterY(72);
    EngineMock engine;
    target.loadCostume(&costume);
    target.setEngine(&engine);

    target.setWidth(14.3);
    target.setHeight(5.8);
    target.setX(64.5);
    target.setY(-43.7);
    target.setZ(2.5);
    target.setRotation(-78.05);
    target.setTransformOriginPoint(QPointF(3.4, 9.7));

    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(544));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(249));
    target.loadProperties();
    ASSERT_EQ(target.width(), 14.3);
    ASSERT_EQ(target.height(), 5.8);
    ASSERT_EQ(target.x(), 64.5);
    ASSERT_EQ(target.y(), -43.7);
    ASSERT_EQ(target.z(), 2.5);
    ASSERT_EQ(target.rotation(), -78.05);
    ASSERT_EQ(target.transformOriginPoint(), QPointF(3.4, 9.7));

    target.updateProperties();
    ASSERT_EQ(target.width(), 4);
    ASSERT_EQ(target.height(), 6);
    ASSERT_EQ(target.x(), 295);
    ASSERT_EQ(target.y(), 52.5);
    ASSERT_EQ(target.z(), 0);
    ASSERT_EQ(target.rotation(), 0);
    ASSERT_EQ(target.transformOriginPoint(), QPointF(-23, 72));

    target.setStageModel(nullptr);
    ASSERT_TRUE(mirrorHorizontallySpy.empty());

    // Sprite
    Sprite sprite;
    sprite.setVisible(true);
    sprite.setRotationStyle(Sprite::RotationStyle::AllAround);
    sprite.setDirection(-67.16);
    sprite.setSize(143.98);
    sprite.setX(-67.94);
    sprite.setY(121.76);
    sprite.setLayerOrder(3);
    SpriteModel spriteModel;
    sprite.setInterface(&spriteModel);
    target.setSpriteModel(&spriteModel);

    target.setWidth(14.3);
    target.setHeight(5.8);
    target.setX(64.5);
    target.setY(-43.7);
    target.setZ(2.5);
    target.setRotation(-78.05);
    target.setTransformOriginPoint(QPointF(3.4, 9.7));

    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(544));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(249));
    target.loadProperties();
    ASSERT_EQ(target.width(), 14.3);
    ASSERT_EQ(target.height(), 5.8);
    ASSERT_EQ(target.x(), 64.5);
    ASSERT_EQ(target.y(), -43.7);
    ASSERT_EQ(target.z(), 2.5);
    ASSERT_EQ(target.rotation(), -78.05);
    ASSERT_EQ(target.transformOriginPoint(), QPointF(3.4, 9.7));

    target.updateProperties();
    ASSERT_EQ(target.width(), 5.7592);
    ASSERT_EQ(target.height(), 8.6388);
    ASSERT_EQ(std::round(target.x() * 100) / 100, 237.18);
    ASSERT_EQ(std::round(target.y() * 100) / 100, -100.93);
    ASSERT_EQ(target.z(), 3);
    ASSERT_EQ(target.rotation(), -157.16);
    ASSERT_EQ(target.transformOriginPoint().x(), -33.1154);
    ASSERT_EQ(std::round(target.transformOriginPoint().y() * 100) / 100, 103.67);
    ASSERT_TRUE(mirrorHorizontallySpy.empty());

    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(544));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(249));
    sprite.setRotationStyle(Sprite::RotationStyle::LeftRight);
    target.loadProperties();
    ASSERT_EQ(target.mirrorHorizontally(), false);
    ASSERT_EQ(target.rotation(), -157.16);
    ASSERT_TRUE(mirrorHorizontallySpy.empty());

    target.updateProperties();
    ASSERT_EQ(target.mirrorHorizontally(), true);
    ASSERT_EQ(target.rotation(), 0);
    ASSERT_EQ(mirrorHorizontallySpy.count(), 1);

    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(544));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(249));
    sprite.setDirection(134.89);
    target.loadProperties();
    ASSERT_EQ(target.mirrorHorizontally(), true);
    ASSERT_EQ(target.rotation(), 0);
    ASSERT_EQ(mirrorHorizontallySpy.count(), 1);

    target.updateProperties();
    ASSERT_EQ(target.mirrorHorizontally(), false);
    ASSERT_EQ(target.rotation(), 0);
    ASSERT_EQ(mirrorHorizontallySpy.count(), 2);

    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(544));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(249));
    sprite.setRotationStyle(Sprite::RotationStyle::DoNotRotate);
    target.loadProperties();
    ASSERT_EQ(target.mirrorHorizontally(), false);
    ASSERT_EQ(target.rotation(), 0);
    ASSERT_EQ(mirrorHorizontallySpy.count(), 2);

    target.updateProperties();
    ASSERT_EQ(target.mirrorHorizontally(), false);
    ASSERT_EQ(target.rotation(), 0);
    ASSERT_EQ(mirrorHorizontallySpy.count(), 2);
}

TEST_F(RenderedTargetTest, LoadJpegCostume)
{
    std::string str = readFileStr("image.jpg");
    Costume costume("", "abc", "jpg");
    costume.setData(str.size(), static_cast<void *>(const_cast<char *>(str.c_str())));
    costume.setBitmapResolution(3);

    RenderedTarget target;

    target.loadCostume(&costume);
    ASSERT_FALSE(target.isSvg());
    ASSERT_FALSE(target.bitmapBuffer()->isOpen());
    target.bitmapBuffer()->open(QBuffer::ReadOnly);
    ASSERT_TRUE(target.bitmapBuffer()->readAll().toStdString().empty());
    ASSERT_TRUE(target.bitmapUniqueKey().toStdString().empty());
    target.bitmapBuffer()->close();

    EngineMock engine;
    SpriteModel spriteModel;
    Sprite sprite;
    sprite.setSize(196.5);
    spriteModel.init(&sprite);

    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    target.setEngine(&engine);
    target.setSpriteModel(&spriteModel);
    target.loadProperties();
    ASSERT_FALSE(target.isSvg());
    ASSERT_FALSE(target.bitmapBuffer()->isOpen());
    target.bitmapBuffer()->open(QBuffer::ReadOnly);
    ASSERT_EQ(target.bitmapBuffer()->readAll().toStdString(), str);
    ASSERT_EQ(target.bitmapUniqueKey().toStdString(), costume.id());
    target.bitmapBuffer()->close();
    target.bitmapBuffer()->open(QBuffer::WriteOnly); // clear the buffer
    target.bitmapBuffer()->close();

    StageModel stageModel;
    Stage stage;

    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    target.setSpriteModel(nullptr);
    target.setStageModel(&stageModel);
    target.loadProperties();
    ASSERT_FALSE(target.isSvg());
    ASSERT_FALSE(target.bitmapBuffer()->isOpen());
    target.bitmapBuffer()->open(QBuffer::ReadOnly);
    ASSERT_EQ(target.bitmapBuffer()->readAll().toStdString(), str);
    ASSERT_EQ(target.bitmapUniqueKey().toStdString(), costume.id());
    target.bitmapBuffer()->close();

    target.updateProperties();
    ASSERT_FALSE(target.isSvg());
    ASSERT_FALSE(target.bitmapBuffer()->isOpen());
    target.bitmapBuffer()->open(QBuffer::ReadOnly);
    ASSERT_EQ(target.bitmapBuffer()->readAll().toStdString(), str);
    ASSERT_EQ(target.bitmapUniqueKey().toStdString(), costume.id());
    target.bitmapBuffer()->close();
}

TEST_F(RenderedTargetTest, LoadPngCostume)
{
    std::string str = readFileStr("image.png");
    Costume costume("", "abc", "png");
    costume.setData(str.size(), static_cast<void *>(const_cast<char *>(str.c_str())));
    costume.setBitmapResolution(3);

    RenderedTarget target;

    target.loadCostume(&costume);
    ASSERT_FALSE(target.isSvg());
    ASSERT_FALSE(target.bitmapBuffer()->isOpen());
    target.bitmapBuffer()->open(QBuffer::ReadOnly);
    ASSERT_TRUE(target.bitmapBuffer()->readAll().toStdString().empty());
    ASSERT_TRUE(target.bitmapUniqueKey().toStdString().empty());
    target.bitmapBuffer()->close();

    EngineMock engine;
    SpriteModel spriteModel;
    Sprite sprite;
    sprite.setSize(196.5);
    spriteModel.init(&sprite);

    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    target.setEngine(&engine);
    target.setSpriteModel(&spriteModel);
    target.loadProperties();
    ASSERT_FALSE(target.isSvg());
    ASSERT_FALSE(target.bitmapBuffer()->isOpen());
    target.bitmapBuffer()->open(QBuffer::ReadOnly);
    ASSERT_EQ(target.bitmapBuffer()->readAll().toStdString(), str);
    ASSERT_EQ(target.bitmapUniqueKey().toStdString(), costume.id());
    target.bitmapBuffer()->close();
    target.bitmapBuffer()->open(QBuffer::WriteOnly); // clear the buffer
    target.bitmapBuffer()->close();

    StageModel stageModel;
    Stage stage;

    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    target.setSpriteModel(nullptr);
    target.setStageModel(&stageModel);
    target.loadProperties();
    ASSERT_FALSE(target.isSvg());
    ASSERT_FALSE(target.bitmapBuffer()->isOpen());
    target.bitmapBuffer()->open(QBuffer::ReadOnly);
    ASSERT_EQ(target.bitmapBuffer()->readAll().toStdString(), str);
    ASSERT_EQ(target.bitmapUniqueKey().toStdString(), costume.id());
    target.bitmapBuffer()->close();

    target.updateProperties();
    ASSERT_FALSE(target.isSvg());
    ASSERT_FALSE(target.bitmapBuffer()->isOpen());
    target.bitmapBuffer()->open(QBuffer::ReadOnly);
    ASSERT_EQ(target.bitmapBuffer()->readAll().toStdString(), str);
    ASSERT_EQ(target.bitmapUniqueKey().toStdString(), costume.id());
    target.bitmapBuffer()->close();
}

TEST_F(RenderedTargetTest, LoadSvgCostume)
{
    // Get maximum viewport dimensions
    QOpenGLContext context;
    context.create();
    Q_ASSERT(context.isValid());

    QOffscreenSurface surface;
    surface.create();
    Q_ASSERT(surface.isValid());

    context.makeCurrent(&surface);
    GLint dims[2];
    glGetIntegerv(GL_MAX_VIEWPORT_DIMS, dims);
    double maxWidth = dims[0] * 0.1;
    double maxHeight = dims[1] * 0.1;
    double maxSize = std::min(maxWidth / (1143 / 90.0), maxHeight / (1143 / 90.0));
    context.doneCurrent();

    std::string str = readFileStr("image.svg");
    auto costume = std::make_shared<Costume>("", "abc", "svg");
    costume->setData(str.size(), static_cast<void *>(const_cast<char *>(str.c_str())));
    costume->setBitmapResolution(1);

    EngineMock engine;
    SpriteModel model;
    Sprite sprite;
    sprite.setSize(maxSize * 100);
    sprite.setX(49.7);
    sprite.setY(-64.15);
    costume->setRotationCenterX(-84);
    costume->setRotationCenterY(53);
    model.init(&sprite);

    RenderedTarget target;
    target.setEngine(&engine);
    target.setSpriteModel(&model);

    target.loadCostume(costume.get());
    ASSERT_TRUE(target.isSvg());
    ASSERT_FALSE(target.bitmapBuffer()->isOpen());
    target.bitmapBuffer()->open(QBuffer::ReadOnly);
    ASSERT_TRUE(target.bitmapBuffer()->readAll().toStdString().empty());
    ASSERT_TRUE(target.bitmapUniqueKey().toStdString().empty());
    target.bitmapBuffer()->close();

    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    target.loadProperties();
    ASSERT_TRUE(target.isSvg());
    ASSERT_FALSE(target.bitmapBuffer()->isOpen());
    target.bitmapBuffer()->open(QBuffer::ReadOnly);
    ASSERT_TRUE(target.bitmapBuffer()->readAll().toStdString().empty());
    ASSERT_TRUE(target.bitmapUniqueKey().toStdString().empty());
    target.bitmapBuffer()->close();

    target.updateProperties();
    ASSERT_TRUE(target.isSvg());
    ASSERT_FALSE(target.bitmapBuffer()->isOpen());
    target.bitmapBuffer()->open(QBuffer::ReadOnly);
    ASSERT_TRUE(target.bitmapBuffer()->readAll().toStdString().empty());
    ASSERT_TRUE(target.bitmapUniqueKey().toStdString().empty());
    target.bitmapBuffer()->close();

    ASSERT_EQ(std::round(target.width() * 100) / 100, maxWidth);
    ASSERT_EQ(std::round(target.height() * 100) / 100, maxHeight);
    ASSERT_EQ(target.scale(), 1);
    ASSERT_EQ(std::round(target.x() * 100) / 100, 11126.36);
    ASSERT_EQ(std::round(target.y() * 100) / 100, -6593.27);
    ASSERT_EQ(std::round(target.transformOriginPoint().x() * 100) / 100, -10836.66);
    ASSERT_EQ(std::round(target.transformOriginPoint().y() * 100) / 100, 6837.42);

    // Test scale limit
    sprite.setSize(maxSize * 250);

    target.loadCostume(costume.get());
    ASSERT_TRUE(target.isSvg());
    ASSERT_FALSE(target.bitmapBuffer()->isOpen());
    target.bitmapBuffer()->open(QBuffer::ReadOnly);
    ASSERT_TRUE(target.bitmapBuffer()->readAll().toStdString().empty());
    ASSERT_TRUE(target.bitmapUniqueKey().toStdString().empty());
    target.bitmapBuffer()->close();

    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    target.loadProperties();
    ASSERT_TRUE(target.isSvg());
    ASSERT_FALSE(target.bitmapBuffer()->isOpen());
    target.bitmapBuffer()->open(QBuffer::ReadOnly);
    ASSERT_TRUE(target.bitmapBuffer()->readAll().toStdString().empty());
    ASSERT_TRUE(target.bitmapUniqueKey().toStdString().empty());
    target.bitmapBuffer()->close();

    target.updateProperties();
    ASSERT_TRUE(target.isSvg());
    ASSERT_FALSE(target.bitmapBuffer()->isOpen());
    target.bitmapBuffer()->open(QBuffer::ReadOnly);
    ASSERT_TRUE(target.bitmapBuffer()->readAll().toStdString().empty());
    ASSERT_TRUE(target.bitmapUniqueKey().toStdString().empty());
    target.bitmapBuffer()->close();

    ASSERT_EQ(std::round(target.width() * 100) / 100, maxWidth);
    ASSERT_EQ(std::round(target.height() * 100) / 100, maxHeight);
    ASSERT_EQ(std::round(target.scale() * 100) / 100, 2.5);
    ASSERT_EQ(std::round(target.x() * 100) / 100, 11126.36);
    ASSERT_EQ(std::round(target.y() * 100) / 100, -6593.27);
    ASSERT_EQ(std::round(target.transformOriginPoint().x() * 100) / 100, -10836.66);
    ASSERT_EQ(std::round(target.transformOriginPoint().y() * 100) / 100, 6837.42);
}

TEST_F(RenderedTargetTest, PaintSvg)
{
    std::string str = readFileStr("image.svg");
    Costume costume("", "abc", "svg");
    costume.setData(str.size(), static_cast<void *>(const_cast<char *>(str.c_str())));
    costume.setBitmapResolution(3);

    EngineMock engine;
    Sprite sprite;
    sprite.setSize(2525.7);

    SpriteModel model;
    model.init(&sprite);

    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    RenderedTarget target;
    target.setEngine(&engine);
    target.setSpriteModel(&model);
    target.loadCostume(&costume);
    target.loadProperties();
    target.updateProperties();

    // Create OpenGL context
    QOpenGLContext context;
    QOffscreenSurface surface;
    createContextAndSurface(&context, &surface);

    // Create a painter
    QNanoPainter painter;

    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);

    // Begin painting
    QOpenGLFramebufferObject fbo(100, 100, format);
    fbo.bind();
    painter.beginFrame(fbo.width(), fbo.height());

    // Paint
    target.paintSvg(&painter);
    painter.endFrame();

    // Compare with reference image
    QBuffer buffer;
    fbo.toImage().save(&buffer, "png");
    QFile ref("svg_result.png");
    ref.open(QFile::ReadOnly);
    buffer.open(QBuffer::ReadOnly);
    ASSERT_EQ(buffer.readAll(), ref.readAll());

    // Release
    fbo.release();
    context.doneCurrent();
}

TEST_F(RenderedTargetTest, HullPoints)
{
    EngineMock engine;
    Sprite sprite;
    SpriteModel model;
    model.init(&sprite);

    RenderedTarget target;
    target.setEngine(&engine);
    target.setSpriteModel(&model);

    // Create OpenGL context
    QOpenGLContext context;
    QOffscreenSurface surface;
    createContextAndSurface(&context, &surface);

    // Create a painter
    QNanoPainter painter;

    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);

    // Begin painting
    QOpenGLFramebufferObject fbo(4, 6, format);
    fbo.bind();
    painter.beginFrame(fbo.width(), fbo.height());

    // Paint
    QNanoImage image = QNanoImage::fromCache(&painter, "image.png");
    painter.drawImage(image, 0, 0);
    painter.endFrame();

    // Test hull points
    target.updateHullPoints(&fbo);
    ASSERT_EQ(target.hullPoints(), std::vector<QPointF>({ { 1, 1 }, { 2, 1 }, { 3, 1 }, { 1, 2 }, { 3, 2 }, { 1, 3 }, { 2, 3 }, { 3, 3 } }));

    // Begin painting (multisampled)
    format.setSamples(16);
    QOpenGLFramebufferObject fboMultiSampled(4, 6, format);
    fboMultiSampled.bind();
    painter.beginFrame(fboMultiSampled.width(), fboMultiSampled.height());

    // Paint (multisampled)
    painter.drawImage(image, 0, 0);
    painter.endFrame();

    // Test hull points (this is undefined with multisampling, so we just check if there are any hull points)
    ASSERT_FALSE(target.hullPoints().empty());

    // Release
    fbo.release();
    context.doneCurrent();
}

TEST_F(RenderedTargetTest, Engine)
{
    RenderedTarget target;
    ASSERT_EQ(target.engine(), nullptr);

    EngineMock engine;
    target.setEngine(&engine);
    ASSERT_EQ(target.engine(), &engine);
}

TEST_F(RenderedTargetTest, StageModel)
{
    RenderedTarget target;
    ASSERT_EQ(target.stageModel(), nullptr);

    StageModel model;
    target.setStageModel(&model);
    ASSERT_EQ(target.stageModel(), &model);
}

TEST_F(RenderedTargetTest, SpriteModel)
{
    RenderedTarget target;
    ASSERT_EQ(target.spriteModel(), nullptr);

    SpriteModel model;
    target.setSpriteModel(&model);
    ASSERT_EQ(target.spriteModel(), &model);
}

TEST_F(RenderedTargetTest, ScratchTarget)
{
    RenderedTarget target;
    ASSERT_EQ(target.scratchTarget(), nullptr);

    StageModel stageModel;
    Stage stage;
    stageModel.init(&stage);
    target.setStageModel(&stageModel);
    ASSERT_EQ(target.scratchTarget(), &stage);

    target.setStageModel(nullptr);
    SpriteModel spriteModel;
    Sprite sprite;
    spriteModel.init(&sprite);
    target.setSpriteModel(&spriteModel);
    ASSERT_EQ(target.scratchTarget(), &sprite);
}
