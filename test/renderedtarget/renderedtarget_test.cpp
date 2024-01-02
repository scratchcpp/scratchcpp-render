#include <QtTest/QSignalSpy>
#include <QOpenGLContext>
#include <QOffscreenSurface>
#include <qnanopainter.h>
#include <renderedtarget.h>
#include <stagemodel.h>
#include <spritemodel.h>
#include <scenemousearea.h>
#include <scratchcpp/stage.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/costume.h>
#include <enginemock.h>

#include "../common.h"

using namespace scratchcpprender;
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

TEST_F(RenderedTargetTest, UpdateMethods)
{
    RenderedTarget parent; // a parent item is needed for setVisible() to work
    RenderedTarget target(&parent);
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
    costume.setBitmapResolution(2.5);
    EngineMock engine;
    target.setEngine(&engine);

    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    target.loadCostume(&costume);
    target.beforeRedraw();
    ASSERT_EQ(target.width(), 1.6);
    ASSERT_EQ(target.height(), 2.4);
    ASSERT_EQ(target.x(), 249.2);
    ASSERT_EQ(target.y(), 151.2);
    ASSERT_EQ(target.z(), 0);
    ASSERT_EQ(target.rotation(), 0);
    ASSERT_EQ(target.transformOriginPoint(), QPointF(-9.2, 28.8));

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

    EXPECT_CALL(engine, stageWidth()).Times(3).WillRepeatedly(Return(480));
    EXPECT_CALL(engine, stageHeight()).Times(3).WillRepeatedly(Return(360));
    target.setSpriteModel(&spriteModel);
    target.beforeRedraw();

    ASSERT_EQ(std::round(target.width() * 100) / 100, 2.3);
    ASSERT_EQ(std::round(target.height() * 100) / 100, 3.46);
    ASSERT_EQ(std::round(target.x() * 100) / 100, 185.31);
    ASSERT_EQ(std::round(target.y() * 100) / 100, 16.77);
    ASSERT_EQ(target.z(), 3);
    ASSERT_EQ(target.rotation(), -157.16);
    ASSERT_EQ(std::round(target.transformOriginPoint().x() * 100) / 100, -13.25);
    ASSERT_EQ(std::round(target.transformOriginPoint().y() * 100) / 100, 41.47);
    ASSERT_TRUE(mirrorHorizontallySpy.empty());

    // Visibility
    target.updateVisibility(false);
    ASSERT_FALSE(target.isVisible());

    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    target.updateVisibility(true);
    ASSERT_TRUE(target.isVisible());

    // X
    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    target.updateX(12.5);
    ASSERT_EQ(std::round(target.x() * 100) / 100, 265.75);
    ASSERT_EQ(std::round(target.y() * 100) / 100, 16.77);

    // Y
    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    target.updateY(-76.09);
    ASSERT_EQ(std::round(target.x() * 100) / 100, 265.75);
    ASSERT_EQ(std::round(target.y() * 100) / 100, 214.62);

    // Size
    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    target.updateSize(56.2);
    target.beforeRedraw();
    ASSERT_EQ(std::round(target.width() * 100) / 100, 0.9);
    ASSERT_EQ(std::round(target.height() * 100) / 100, 1.35);
    ASSERT_EQ(std::round(target.x() * 100) / 100, 257.67);
    ASSERT_EQ(std::round(target.y() * 100) / 100, 239.9);
    ASSERT_EQ(std::round(target.transformOriginPoint().x() * 100) / 100, -5.17);
    ASSERT_EQ(std::round(target.transformOriginPoint().y() * 100) / 100, 16.19);

    // Direction
    target.updateDirection(123.8);
    ASSERT_EQ(target.rotation(), 33.8);
    ASSERT_TRUE(mirrorHorizontallySpy.empty());

    // Rotation style
    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    target.updateRotationStyle(Sprite::RotationStyle::LeftRight);
    ASSERT_EQ(target.mirrorHorizontally(), false);
    ASSERT_EQ(target.rotation(), 0);
    ASSERT_EQ(std::round(target.x() * 100) / 100, 257.67);
    ASSERT_EQ(std::round(target.y() * 100) / 100, 239.9);
    ASSERT_TRUE(mirrorHorizontallySpy.empty());

    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    target.updateDirection(-15);
    ASSERT_EQ(target.mirrorHorizontally(), true);
    ASSERT_EQ(target.rotation(), 0);
    ASSERT_EQ(std::round(target.x() * 100) / 100, 247.33);
    ASSERT_EQ(std::round(target.y() * 100) / 100, 239.9);
    ASSERT_EQ(mirrorHorizontallySpy.count(), 1);

    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    target.updateDirection(134.89);
    ASSERT_EQ(target.mirrorHorizontally(), false);
    ASSERT_EQ(target.rotation(), 0);
    ASSERT_EQ(std::round(target.x() * 100) / 100, 257.67);
    ASSERT_EQ(std::round(target.y() * 100) / 100, 239.9);
    ASSERT_EQ(mirrorHorizontallySpy.count(), 2);

    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    target.updateRotationStyle(Sprite::RotationStyle::DoNotRotate);
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
    ASSERT_EQ(target.bitmapBuffer()->readAll().toStdString(), str);
    ASSERT_EQ(target.bitmapUniqueKey().toStdString(), costume.id());
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
    ASSERT_EQ(target.bitmapBuffer()->readAll().toStdString(), str);
    ASSERT_EQ(target.bitmapUniqueKey().toStdString(), costume.id());
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

    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    target.setSpriteModel(&model);
    target.loadCostume(costume.get());
    target.beforeRedraw();
    ASSERT_TRUE(target.isSvg());
    ASSERT_FALSE(target.bitmapBuffer()->isOpen());
    target.bitmapBuffer()->open(QBuffer::ReadOnly);
    ASSERT_TRUE(target.bitmapBuffer()->readAll().toStdString().empty());
    ASSERT_TRUE(target.bitmapUniqueKey().toStdString().empty());
    target.bitmapBuffer()->close();

    ASSERT_EQ(std::round(target.width() * 100) / 100, 1548.09);
    ASSERT_EQ(std::round(target.height() * 100) / 100, 1548.09);
    ASSERT_EQ(target.scale(), 1);
    ASSERT_EQ(std::round(target.x() * 100) / 100, 11126.36);
    ASSERT_EQ(std::round(target.y() * 100) / 100, -6593.27);
    ASSERT_EQ(std::round(target.transformOriginPoint().x() * 100) / 100, -10836.66);
    ASSERT_EQ(std::round(target.transformOriginPoint().y() * 100) / 100, 6837.42);

    // Test scale limit
    EXPECT_CALL(engine, stageWidth()).Times(2).WillRepeatedly(Return(480));
    EXPECT_CALL(engine, stageHeight()).Times(2).WillRepeatedly(Return(360));
    target.updateSize(maxSize * 250);
    target.setStageScale(3.89);

    ASSERT_EQ(std::round(target.width() * 100) / 100, 1548.09);
    ASSERT_EQ(std::round(target.height() * 100) / 100, 1548.09);
    ASSERT_EQ(std::round(target.scale() * 100) / 100, 9.19);
    ASSERT_EQ(std::round(target.x() * 100) / 100, 12595.73);
    ASSERT_EQ(std::round(target.y() * 100) / 100, -6286.52);
    ASSERT_EQ(std::round(target.transformOriginPoint().x() * 100) / 100, -11468.8);
    ASSERT_EQ(std::round(target.transformOriginPoint().y() * 100) / 100, 7236.27);
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
    target.beforeRedraw();

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
    target.setWidth(3);
    target.setHeight(3);
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

    // Test contains()
    ASSERT_FALSE(target.contains({ 0, 0 }));
    ASSERT_FALSE(target.contains({ 1, 0 }));
    ASSERT_FALSE(target.contains({ 2, 0 }));
    ASSERT_FALSE(target.contains({ 3, 0 }));

    ASSERT_FALSE(target.contains({ 0, 1 }));
    ASSERT_TRUE(target.contains({ 1, 1 }));
    ASSERT_TRUE(target.contains({ 1.4, 1.25 }));
    ASSERT_TRUE(target.contains({ 2, 1 }));
    ASSERT_TRUE(target.contains({ 3, 1 }));

    ASSERT_TRUE(target.contains({ 1, 2 }));
    ASSERT_FALSE(target.contains({ 2, 2 }));
    ASSERT_TRUE(target.contains({ 3, 2 }));
    ASSERT_FALSE(target.contains({ 3.5, 2.1 }));

    ASSERT_TRUE(target.contains({ 1, 3 }));
    ASSERT_TRUE(target.contains({ 2, 3 }));
    ASSERT_TRUE(target.contains({ 3, 3 }));
    ASSERT_FALSE(target.contains({ 3.3, 3.5 }));
}

TEST_F(RenderedTargetTest, SpriteDragging)
{
    RenderedTarget target;
    EngineMock engine;
    target.setEngine(&engine);

    SpriteModel model;
    Sprite sprite;
    sprite.setX(64.08);
    sprite.setY(-6.86);
    model.init(&sprite);
    target.setSpriteModel(&model);
    target.setStageScale(3.5);

    SceneMouseArea mouseArea;
    target.setMouseArea(&mouseArea);

    emit mouseArea.mouseMoved(1064, 651);
    ASSERT_EQ(sprite.x(), 64.08);
    ASSERT_EQ(sprite.y(), -6.86);
    ASSERT_EQ(mouseArea.draggedSprite(), nullptr);

    // Try right mouse button (should not work)
    QMouseEvent moveEventRightButton(QEvent::MouseMove, QPointF(), QPointF(), Qt::RightButton, Qt::RightButton, Qt::NoModifier);
    QMouseEvent pressEventRightButton(QEvent::MouseButtonPress, QPointF(), QPointF(), Qt::RightButton, Qt::RightButton, Qt::NoModifier);
    QMouseEvent releaseEventRightButton(QEvent::MouseButtonRelease, QPointF(), QPointF(), Qt::RightButton, Qt::RightButton, Qt::NoModifier);
    EXPECT_CALL(engine, clickTarget(&sprite));
    QCoreApplication::sendEvent(&target, &pressEventRightButton);
    QCoreApplication::sendEvent(&target, &moveEventRightButton);
    ASSERT_EQ(sprite.x(), 64.08);
    ASSERT_EQ(sprite.y(), -6.86);
    ASSERT_EQ(mouseArea.draggedSprite(), nullptr);
    EXPECT_CALL(engine, clickTarget).Times(0);
    QCoreApplication::sendEvent(&target, &releaseEventRightButton);
    ASSERT_EQ(mouseArea.draggedSprite(), nullptr);

    emit mouseArea.mouseMoved(1064, 651);
    ASSERT_EQ(sprite.x(), 64.08);
    ASSERT_EQ(sprite.y(), -6.86);
    ASSERT_EQ(mouseArea.draggedSprite(), nullptr);

    // Try right mouse button with "draggable" set to true (should not work)
    sprite.setDraggable(true);
    EXPECT_CALL(engine, clickTarget).Times(0);
    QCoreApplication::sendEvent(&target, &pressEventRightButton);
    QCoreApplication::sendEvent(&target, &moveEventRightButton);
    ASSERT_EQ(sprite.x(), 64.08);
    ASSERT_EQ(sprite.y(), -6.86);
    ASSERT_EQ(mouseArea.draggedSprite(), nullptr);
    EXPECT_CALL(engine, clickTarget(&sprite));
    QCoreApplication::sendEvent(&target, &releaseEventRightButton);
    ASSERT_EQ(mouseArea.draggedSprite(), nullptr);

    emit mouseArea.mouseMoved(1064, 651);
    ASSERT_EQ(sprite.x(), 64.08);
    ASSERT_EQ(sprite.y(), -6.86);
    ASSERT_EQ(mouseArea.draggedSprite(), nullptr);

    // Try left mouse button (should not work with "draggable" set to false)
    sprite.setDraggable(false);
    QMouseEvent moveEvent(QEvent::MouseMove, QPointF(), QPointF(), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QMouseEvent pressEvent(QEvent::MouseButtonPress, QPointF(), QPointF(), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QMouseEvent releaseEvent(QEvent::MouseButtonRelease, QPointF(), QPointF(), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    EXPECT_CALL(engine, clickTarget(&sprite));
    QCoreApplication::sendEvent(&target, &pressEvent);
    QCoreApplication::sendEvent(&target, &moveEvent);
    ASSERT_EQ(sprite.x(), 64.08);
    ASSERT_EQ(sprite.y(), -6.86);
    ASSERT_EQ(mouseArea.draggedSprite(), nullptr);

    emit mouseArea.mouseMoved(1064, 651);
    ASSERT_EQ(sprite.x(), 64.08);
    ASSERT_EQ(sprite.y(), -6.86);
    ASSERT_EQ(mouseArea.draggedSprite(), nullptr);
    EXPECT_CALL(engine, clickTarget).Times(0);
    QCoreApplication::sendEvent(&target, &releaseEvent);

    // Try left mouse button with "draggable" set to true
    sprite.setDraggable(true);
    EXPECT_CALL(engine, clickTarget).Times(0);
    QCoreApplication::sendEvent(&target, &pressEvent);
    EXPECT_CALL(engine, mouseX()).WillOnce(Return(67.95));
    EXPECT_CALL(engine, mouseY()).WillOnce(Return(2.1));
    EXPECT_CALL(engine, moveSpriteToFront(&sprite));
    QCoreApplication::sendEvent(&target, &moveEvent);
    ASSERT_EQ(sprite.x(), 64.08);
    ASSERT_EQ(sprite.y(), -6.86);
    ASSERT_EQ(mouseArea.draggedSprite(), &target);

    // Drag
    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    emit mouseArea.mouseMoved(1067.8, 649.06);
    ASSERT_EQ(std::round(sprite.x() * 100) / 100, 61.22);
    ASSERT_EQ(std::round(sprite.y() * 100) / 100, -14.41);
    ASSERT_EQ(mouseArea.draggedSprite(), &target);

    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    emit mouseArea.mouseMoved(1092.47, 605.46);
    ASSERT_EQ(std::round(sprite.x() * 100) / 100, 68.26);
    ASSERT_EQ(std::round(sprite.y() * 100) / 100, -1.95);
    ASSERT_EQ(mouseArea.draggedSprite(), &target);

    // Create another sprite
    RenderedTarget anotherTarget;
    anotherTarget.setEngine(&engine);

    SpriteModel anotherModel;
    Sprite anotherSprite;
    anotherSprite.setX(64.08);
    anotherSprite.setY(-6.86);
    anotherSprite.setDraggable(true);
    anotherModel.init(&anotherSprite);
    anotherTarget.setSpriteModel(&model);
    anotherTarget.setStageScale(3.5);
    anotherTarget.setMouseArea(&mouseArea);

    // Try to drag the second sprite while the first is being dragged
    sprite.setDraggable(true);
    EXPECT_CALL(engine, clickTarget).Times(0);
    QCoreApplication::sendEvent(&anotherTarget, &pressEvent);
    QCoreApplication::sendEvent(&anotherTarget, &moveEvent);
    ASSERT_EQ(mouseArea.draggedSprite(), &target);
    EXPECT_CALL(engine, clickTarget(&sprite));
    QCoreApplication::sendEvent(&anotherTarget, &releaseEvent);

    // Stop dragging
    EXPECT_CALL(engine, clickTarget).Times(0);
    QCoreApplication::sendEvent(&target, &releaseEvent);
    ASSERT_EQ(std::round(sprite.x() * 100) / 100, 68.26);
    ASSERT_EQ(std::round(sprite.y() * 100) / 100, -1.95);
    ASSERT_EQ(mouseArea.draggedSprite(), nullptr);
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

TEST_F(RenderedTargetTest, MouseArea)
{
    RenderedTarget target;
    ASSERT_EQ(target.mouseArea(), nullptr);

    SceneMouseArea mouseArea;
    target.setMouseArea(&mouseArea);
    ASSERT_EQ(target.mouseArea(), &mouseArea);
}

TEST_F(RenderedTargetTest, StageScale)
{
    RenderedTarget target;
    ASSERT_EQ(target.stageScale(), 1);

    target.setStageScale(6.4);
    ASSERT_EQ(target.stageScale(), 6.4);
}
