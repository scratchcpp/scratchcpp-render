// SPDX-License-Identifier: LGPL-3.0-or-later

#include <scratchcpp/iengine.h>
#include <scratchcpp/costume.h>
#include <scratchcpp/rect.h>
#include <scratchcpp/value.h>
#include <QtSvg/QSvgRenderer>
#include <qnanopainter.h>

#include "renderedtarget.h"
#include "targetpainter.h"
#include "stagemodel.h"
#include "spritemodel.h"
#include "scenemousearea.h"
#include "bitmapskin.h"
#include "svgskin.h"
#include "cputexturemanager.h"
#include "penlayer.h"

using namespace scratchcpprender;
using namespace libscratchcpp;

static const double SVG_SCALE_LIMIT = 0.1; // the maximum viewport dimensions are multiplied by this
static const double pi = std::acos(-1);    // TODO: Use std::numbers::pi in C++20

RenderedTarget::RenderedTarget(QQuickItem *parent) :
    IRenderedTarget(parent)
{
    setSmooth(false);
}

RenderedTarget::~RenderedTarget()
{
    if (!m_skinsInherited) {
        for (const auto &[costume, skin] : m_skins)
            delete skin;
    }
}

void RenderedTarget::updateVisibility(bool visible)
{
    if (visible == isVisible())
        return;

    setVisible(visible);
    calculatePos();
    m_convexHullDirty = true;
}

void RenderedTarget::updateX(double x)
{
    if (x == m_x)
        return;

    m_x = std::round(x);
    calculatePos();
}

void RenderedTarget::updateY(double y)
{
    if (y == m_y)
        return;

    m_y = std::round(y);
    calculatePos();
}

void RenderedTarget::updateSize(double size)
{
    if (size / 100 == m_size)
        return;

    m_size = size / 100;
    calculateSize();
    calculatePos();
}

void RenderedTarget::updateDirection(double direction)
{
    if (direction == m_direction)
        return;

    m_direction = direction;
    calculateRotation();

    if (m_rotationStyle == libscratchcpp::Sprite::RotationStyle::LeftRight)
        calculatePos();
}

void RenderedTarget::updateRotationStyle(libscratchcpp::Sprite::RotationStyle style)
{
    if (style == m_rotationStyle)
        return;

    m_rotationStyle = style;
    calculateRotation();
    calculatePos();
}

void RenderedTarget::updateLayerOrder(int layerOrder)
{
    setZ(layerOrder);
}

void RenderedTarget::updateCostume(Costume *costume)
{
    if (!costume || costume == m_costume)
        return;

    m_costume = costume;

    if (m_costumesLoaded) {
        auto it = m_skins.find(m_costume);

        if (it == m_skins.end())
            m_skin = nullptr;
        else
            m_skin = it->second;
    }

    setSmooth(m_costume->dataFormat() == "svg");

    calculateSize();
    calculatePos();
}

bool RenderedTarget::costumesLoaded() const
{
    return m_costumesLoaded;
}

void RenderedTarget::loadCostumes()
{
    // Delete previous skins
    if (!m_skinsInherited) {
        for (const auto &[costume, skin] : m_skins)
            delete skin;
    }

    m_skinsInherited = false;
    m_skins.clear();

    // Generate a skin for each costume
    Target *target = scratchTarget();

    if (!target)
        return;

    const auto &costumes = target->costumes();

    for (auto costume : costumes) {
        Skin *skin = nullptr;
        if (costume->dataFormat() == "svg")
            skin = new SVGSkin(costume.get());
        else
            skin = new BitmapSkin(costume.get());

        if (skin)
            m_skins[costume.get()] = skin;

        if (m_costume && costume.get() == m_costume)
            m_skin = skin;
    }

    m_costumesLoaded = true;

    if (m_costume) {
        calculateSize();
        calculatePos();
    }
}

void RenderedTarget::beforeRedraw()
{
    // These properties must be set here to avoid unnecessary calls to update()
    setWidth(m_width);
    setHeight(m_height);

    if (!m_oldTexture.isValid() || (m_texture.isValid() && m_texture != m_oldTexture)) {
        m_oldTexture = m_texture;
        update();
    }
}

void RenderedTarget::deinitClone()
{
    // Do not process mouse move events after the clone has been deleted
    disconnect(m_mouseArea, &SceneMouseArea::mouseMoved, this, &RenderedTarget::handleSceneMouseMove);

    // Release drag lock
    if (m_mouseArea->draggedSprite() == this)
        m_mouseArea->setDraggedSprite(nullptr);
}

IEngine *RenderedTarget::engine() const
{
    return m_engine;
}

void RenderedTarget::setEngine(IEngine *newEngine)
{
    if (m_engine == newEngine)
        return;

    m_engine = newEngine;
    m_costume = nullptr;
    m_costumesLoaded = false;

    if (!m_skinsInherited) {
        for (const auto &[costume, skin] : m_skins)
            delete skin;

        m_skins.clear();
    }

    m_skin = nullptr;
    m_texture = Texture();
    m_oldTexture = Texture();
    m_cpuTexture = Texture();
    m_penLayer = PenLayer::getProjectPenLayer(m_engine);
    m_convexHullDirty = true;
    clearGraphicEffects();
    m_hullPoints.clear();

    emit engineChanged();
}

StageModel *RenderedTarget::stageModel() const
{
    return m_stageModel;
}

void RenderedTarget::setStageModel(StageModel *newStageModel)
{
    if (m_stageModel == newStageModel)
        return;

    m_stageModel = newStageModel;

    if (m_stageModel) {
        Stage *stage = m_stageModel->stage();

        if (stage)
            updateCostume(stage->currentCostume().get());
    }

    emit stageModelChanged();
}

SpriteModel *RenderedTarget::spriteModel() const
{
    return m_spriteModel;
}

void RenderedTarget::setSpriteModel(SpriteModel *newSpriteModel)
{
    if (m_spriteModel == newSpriteModel)
        return;

    m_spriteModel = newSpriteModel;

    if (m_spriteModel) {
        SpriteModel *cloneRoot = m_spriteModel->cloneRoot();

        if (cloneRoot) {
            // Inherit skins, texture mananger, convex hull points, etc. from the clone root
            RenderedTarget *target = dynamic_cast<RenderedTarget *>(cloneRoot->renderedTarget());
            Q_ASSERT(target);
            m_textureManager = target->m_textureManager;
            m_convexHullDirty = target->m_convexHullDirty;
            m_hullPoints = target->m_hullPoints;

            if (target->costumesLoaded()) {
                m_skins = target->m_skins; // TODO: Avoid copying - maybe using a pointer?
                m_costumesLoaded = true;
                m_skinsInherited = true; // avoid double free
            }
        }

        Sprite *sprite = m_spriteModel->sprite();

        if (sprite) {
            m_x = sprite->x();
            m_y = sprite->y();
            m_size = sprite->size() / 100;
            m_direction = sprite->direction();
            m_rotationStyle = sprite->rotationStyle();
            updateCostume(sprite->currentCostume().get());
            updateVisibility(sprite->visible());
            updateLayerOrder(sprite->layerOrder());
            calculateSize();
            calculatePos();
            calculateRotation();
        }
    }
    emit spriteModelChanged();
}

Target *RenderedTarget::scratchTarget() const
{
    if (m_spriteModel)
        return m_spriteModel->sprite();
    else if (m_stageModel)
        return m_stageModel->stage();
    else
        return nullptr;
}

SceneMouseArea *RenderedTarget::mouseArea() const
{
    return m_mouseArea;
}

void RenderedTarget::setMouseArea(SceneMouseArea *newMouseArea)
{
    if (m_mouseArea == newMouseArea)
        return;

    m_mouseArea = newMouseArea;
    Q_ASSERT(m_mouseArea);
    connect(m_mouseArea, &SceneMouseArea::mouseMoved, this, &RenderedTarget::handleSceneMouseMove);
    emit mouseAreaChanged();
}

double RenderedTarget::stageScale() const
{
    return m_stageScale;
}

void RenderedTarget::setStageScale(double newStageScale)
{
    if (qFuzzyCompare(m_stageScale, newStageScale))
        return;

    m_stageScale = newStageScale;
    calculateSize();
    calculatePos();
    beforeRedraw();
    emit stageScaleChanged();
}

qreal RenderedTarget::width() const
{
    return QNanoQuickItem::width();
}

void RenderedTarget::setWidth(qreal width)
{
    QNanoQuickItem::setWidth(width);
}

qreal RenderedTarget::height() const
{
    return QNanoQuickItem::height();
}

void RenderedTarget::setHeight(qreal height)
{
    QNanoQuickItem::setHeight(height);
}

Rect RenderedTarget::getBounds() const
{
    // https://github.com/scratchfoundation/scratch-render/blob/c3ede9c3d54769730c7b023021511e2aba167b1f/src/Rectangle.js#L33-L55
    if (!m_costume || !m_skin || !m_texture.isValid() || !m_cpuTexture.isValid())
        return Rect(m_x, m_y, m_x, m_y);

    const double textureScale = m_skin->getTextureScale(m_cpuTexture);
    const double bitmapRes = m_costume->bitmapResolution();
    const double width = m_cpuTexture.width() * m_size / textureScale;
    const double height = m_cpuTexture.height() * m_size / textureScale;
    const double originX = m_costume->rotationCenterX() * m_size / bitmapRes - width / 2;
    const double originY = -m_costume->rotationCenterY() * m_size / bitmapRes + height / 2;
    const double rot = -rotation() * pi / 180;
    const double sinRot = std::sin(rot);
    const double cosRot = std::cos(rot);
    double left = std::numeric_limits<double>::infinity();
    double top = -std::numeric_limits<double>::infinity();
    double right = -std::numeric_limits<double>::infinity();
    double bottom = std::numeric_limits<double>::infinity();

    const std::vector<QPoint> &points = hullPoints();

    for (const QPointF &point : points) {
        double x = point.x() * m_size / textureScale / bitmapRes - width / 2;
        double y = height / 2 - point.y() * m_size / textureScale / bitmapRes;
        const QPointF transformed = transformPoint(x, y, originX, originY, sinRot, cosRot);
        x = transformed.x() * (m_mirrorHorizontally ? -1 : 1);
        y = transformed.y();

        if (x < left)
            left = x;

        if (x > right)
            right = x;

        if (y > top)
            top = y;

        if (y < bottom)
            bottom = y;
    }

    return Rect(left + m_x, top + m_y, right + m_x, bottom + m_y);
}

QRectF scratchcpprender::RenderedTarget::getQmlBounds() const
{
    Rect bounds = getBounds();
    return QRectF(QPointF(bounds.left(), bounds.top()), QPointF(bounds.right(), bounds.bottom()));
}

QRectF RenderedTarget::getBoundsForBubble() const
{
    // https://github.com/scratchfoundation/scratch-render/blob/86dcb0151a04bc8c1ff39559e8531e7921102b56/src/Drawable.js#L536-L551
    Rect rect = getBounds();
    const int slice = 8; // px, how tall the top slice to measure should be

    if (rect.height() > slice)
        rect.setBottom(rect.top() - slice);

    Q_ASSERT(rect.height() <= 8 || std::abs(rect.bottom()) == std::numeric_limits<double>::infinity() || std::abs(rect.top()) == std::numeric_limits<double>::infinity());
    return QRectF(QPointF(rect.left(), rect.top()), QPointF(rect.right(), rect.bottom()));
}

Rect RenderedTarget::getFastBounds() const
{
    if (!m_costume || !m_skin || !m_texture.isValid() || !m_cpuTexture.isValid())
        return Rect(m_x, m_y, m_x, m_y);

    const double textureScale = m_skin->getTextureScale(m_cpuTexture);
    const double bitmapRes = m_costume->bitmapResolution();
    const double width = m_cpuTexture.width() * m_size / textureScale / bitmapRes;
    const double height = m_cpuTexture.height() * m_size / textureScale / bitmapRes;
    const double originX = m_costume->rotationCenterX() * m_size / bitmapRes - width / 2;
    const double originY = -m_costume->rotationCenterY() * m_size / bitmapRes + height / 2;
    const double rot = -rotation() * pi / 180;
    const double sinRot = std::sin(rot);
    const double cosRot = std::cos(rot);

    QPointF topLeft = transformPoint(-width / 2, height / 2, originX, originY, sinRot, cosRot);
    QPointF topRight = transformPoint(width / 2, height / 2, originX, originY, sinRot, cosRot);
    QPointF bottomRight = transformPoint(width / 2, -height / 2, originX, originY, sinRot, cosRot);
    QPointF bottomLeft = transformPoint(-width / 2, -height / 2, originX, originY, sinRot, cosRot);

    if (m_mirrorHorizontally) {
        topLeft.setX(-topLeft.x());
        topRight.setX(-topRight.x());
        bottomRight.setX(-bottomRight.x());
        bottomLeft.setX(-bottomLeft.x());
    }
    const auto xList = { topLeft.x(), topRight.x(), bottomRight.x(), bottomLeft.x() };
    const auto yList = { topLeft.y(), topRight.y(), bottomRight.y(), bottomLeft.y() };
    const double minX = std::min(xList);
    const double maxX = std::max(xList);
    const double minY = std::min(yList);
    const double maxY = std::max(yList);

    return Rect(minX + m_x, maxY + m_y, maxX + m_x, minY + m_y);
}

QPointF RenderedTarget::mapFromScene(const QPointF &point) const
{
    return QNanoQuickItem::mapFromScene(point);
}

QNanoQuickItemPainter *RenderedTarget::createItemPainter() const
{
    return new TargetPainter();
}

void RenderedTarget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        m_clicked = true;

    if (m_engine && (!m_spriteModel || !m_spriteModel->sprite()->draggable())) {
        // Notify libscratchcpp about the click
        m_engine->clickTarget(scratchTarget());
    }
}

void RenderedTarget::mouseReleaseEvent(QMouseEvent *event)
{
    m_clicked = false;
    Q_ASSERT(m_mouseArea);

    // Stop dragging
    if (m_mouseArea->draggedSprite() == this)
        m_mouseArea->setDraggedSprite(nullptr);
    else if (m_engine && m_spriteModel && m_spriteModel->sprite()->draggable()) {
        // Notify libscratchcpp about the click
        m_engine->clickTarget(scratchTarget());
    }
}

void RenderedTarget::mouseMoveEvent(QMouseEvent *event)
{
    Q_ASSERT((m_spriteModel && m_spriteModel->sprite()) || m_stageModel);
    Q_ASSERT(m_mouseArea);

    // Start dragging
    if (m_clicked && !m_mouseArea->draggedSprite() && m_spriteModel && m_spriteModel->sprite()->draggable()) {
        Q_ASSERT(m_engine);
        Sprite *sprite = m_spriteModel->sprite();
        m_dragDeltaX = m_engine->mouseX() - sprite->x();
        m_dragDeltaY = m_engine->mouseY() - sprite->y();
        m_mouseArea->setDraggedSprite(this);

        // Move the sprite to the front layer
        m_engine->moveSpriteToFront(sprite);
    }
}

Texture RenderedTarget::texture() const
{
    return m_texture;
}

const std::unordered_map<ShaderManager::Effect, double> &RenderedTarget::graphicEffects() const
{
    return m_graphicEffects;
}

void RenderedTarget::setGraphicEffect(ShaderManager::Effect effect, double value)
{
    bool changed = false;
    auto it = m_graphicEffects.find(effect);

    if (value == 0) {
        if (it != m_graphicEffects.cend()) {
            changed = true;
            m_graphicEffects.erase(effect);
        }
    } else {
        if (it != m_graphicEffects.cend())
            changed = it->second != value;
        else
            changed = true;

        m_graphicEffects[effect] = value;
    }

    if (changed)
        update();

    // TODO: Set m_convexHullDirty to true if the effect changes shape
}

void RenderedTarget::clearGraphicEffects()
{
    if (!m_graphicEffects.empty())
        update();

    // TODO: Set m_convexHullDirty to true if any of the previous effects changed shape
    m_graphicEffects.clear();
}

const std::vector<QPoint> &RenderedTarget::hullPoints() const
{
    if (convexHullPointsNeeded())
        const_cast<RenderedTarget *>(this)->updateHullPoints();

    return m_hullPoints;
}

bool RenderedTarget::contains(const QPointF &point) const
{
    if (!m_costume || !m_texture.isValid() || !m_cpuTexture.isValid() || !parentItem())
        return false;

    const double scaleRatio = m_skin->getTextureScale(m_texture) / m_skin->getTextureScale(m_cpuTexture);
    QPointF translatedPoint = mapToItem(parentItem(), point);
    translatedPoint = mapFromStageWithOriginPoint(translatedPoint);
    translatedPoint /= scaleRatio;

    return containsLocalPoint(translatedPoint);
}

bool RenderedTarget::containsScratchPoint(double x, double y) const
{
    if (!m_engine)
        return false;

    return containsLocalPoint(mapFromScratchToLocal(QPointF(x, y)));
}

QRgb RenderedTarget::colorAtScratchPoint(double x, double y) const
{
    // NOTE: Only this target is processed! Use sampleColor3b() to get the final color.
    if (!m_engine || !m_cpuTexture.isValid())
        return qRgba(0, 0, 0, 0);

    // Translate the coordinates
    QPointF point = mapFromScratchToLocal(QPointF(x, y));
    x = std::floor(point.x());
    y = std::floor(point.y());

    const double width = m_cpuTexture.width();
    const double height = m_cpuTexture.height();

    // If the point is outside the texture, return fully transparent color
    if ((x < 0 || x >= width) || (y < 0 || y >= height))
        return qRgba(0, 0, 0, 0);

    GLubyte *data = textureManager()->getTextureData(m_cpuTexture);
    const int index = (y * width + x) * 4; // RGBA channels
    Q_ASSERT(index >= 0 && index < width * height * 4);
    // TODO: Apply graphic effects (#117)
    return qRgba(data[index], data[index + 1], data[index + 2], data[index + 3]);
}

bool RenderedTarget::touchingClones(const std::vector<libscratchcpp::Sprite *> &clones) const
{
    // https://github.com/scratchfoundation/scratch-render/blob/941562438fe3dd6e7d98d9387607d535dcd68d24/src/RenderWebGL.js#L967-L1002
    // TODO: Use Rect methods and do not use QRects
    const QRectF myRect = touchingBounds();

    if (myRect.isEmpty())
        return false;

    std::vector<IRenderedTarget *> candidates;

    // Calculate the union of the bounding rectangle intersections
    QRectF united = candidatesBounds(myRect, clones, candidates);

    if (united.isEmpty() || candidates.empty())
        return false;

    // Loop through the points of the union
    for (int y = united.top(); y <= united.bottom(); y++) {
        for (int x = united.left(); x <= united.right(); x++) {
            if (this->containsScratchPoint(x, y)) {
                for (IRenderedTarget *candidate : candidates) {
                    if (candidate->containsScratchPoint(x, y))
                        return true;
                }
            }
        }
    }

    return false;
}

bool RenderedTarget::touchingColor(const Value &color) const
{
    // https://github.com/scratchfoundation/scratch-render/blob/0a04c2fb165f5c20406ec34ab2ea5682ae45d6e0/src/RenderWebGL.js#L775-L841
    QRgb rgb = convertColor(color);

    std::vector<Target *> targets;
    getVisibleTargets(targets);

    QRectF myRect = touchingBounds();
    std::vector<IRenderedTarget *> candidates;
    QRectF bounds = candidatesBounds(myRect, targets, candidates);

    if (colorMatches(rgb, qRgb(255, 255, 255))) {
        // The color we're checking for is the background color which spans the entire stage
        bounds = myRect;

        if (bounds.isEmpty())
            return false;
    } else if (candidates.empty()) {
        // If not checking for the background color, we can return early if there are no candidate drawables
        return false;
    }

    // Loop through the points of the union
    for (int y = bounds.top(); y <= bounds.bottom(); y++) {
        for (int x = bounds.left(); x <= bounds.right(); x++) {
            if (this->containsScratchPoint(x, y)) {
                QRgb pixelColor = sampleColor3b(x, y, candidates);

                if (colorMatches(rgb, pixelColor))
                    return true;
            }
        }
    }

    return false;
}

void RenderedTarget::calculatePos()
{
    if (!m_skin || !m_costume || !m_engine)
        return;

    if (isVisible() || m_stageModel) {
        double stageWidth = m_engine->stageWidth();
        double stageHeight = m_engine->stageHeight();
        setX(m_stageScale * (stageWidth / 2 + m_x - m_costume->rotationCenterX() * m_size / scale() / m_costume->bitmapResolution() * (m_mirrorHorizontally ? -1 : 1)));
        setY(m_stageScale * (stageHeight / 2 - m_y - m_costume->rotationCenterY() * m_size / scale() / m_costume->bitmapResolution()));
        qreal originX = m_costume->rotationCenterX() * m_stageScale * m_size / scale() / m_costume->bitmapResolution();
        qreal originY = m_costume->rotationCenterY() * m_stageScale * m_size / scale() / m_costume->bitmapResolution();
        setTransformOriginPoint(QPointF(originX, originY));

        // Qt ignores the transform origin point if it's (0, 0),
        // so set the transform origin to top left in this case.
        if (originX == 0 && originY == 0)
            setTransformOrigin(QQuickItem::TopLeft);
        else
            setTransformOrigin(QQuickItem::Center);
    }
}

void RenderedTarget::calculateRotation()
{
    if (isVisible()) {
        // Direction
        bool oldMirrorHorizontally = m_mirrorHorizontally;

        switch (m_rotationStyle) {
            case Sprite::RotationStyle::AllAround:
                setRotation(m_direction - 90);
                m_mirrorHorizontally = (false);

                break;

            case Sprite::RotationStyle::LeftRight: {
                setRotation(0);
                m_mirrorHorizontally = (m_direction < 0);

                break;
            }

            case Sprite::RotationStyle::DoNotRotate:
                setRotation(0);
                m_mirrorHorizontally = false;
                break;
        }

        if (m_mirrorHorizontally != oldMirrorHorizontally)
            emit mirrorHorizontallyChanged();
    }
}

void RenderedTarget::calculateSize()
{
    if (m_skin && m_costume) {
        GLuint oldTexture = m_cpuTexture.handle();
        bool wasValid = m_cpuTexture.isValid();
        m_texture = m_skin->getTexture(m_size * m_stageScale);
        m_cpuTexture = m_skin->getTexture(m_size);
        m_width = m_texture.width();
        m_height = m_texture.height();
        setScale(m_size * m_stageScale / m_skin->getTextureScale(m_texture) / m_costume->bitmapResolution());

        if (wasValid && m_cpuTexture.handle() != oldTexture)
            m_convexHullDirty = true;
    }
}

void RenderedTarget::handleSceneMouseMove(qreal x, qreal y)
{
    Q_ASSERT(m_mouseArea);

    if (m_mouseArea->draggedSprite() == this) {
        Q_ASSERT(m_spriteModel && m_spriteModel->sprite());
        Q_ASSERT(m_engine);
        Sprite *sprite = m_spriteModel->sprite();
        sprite->setX(x / m_stageScale - m_engine->stageWidth() / 2.0 - m_dragDeltaX);
        sprite->setY(-y / m_stageScale + m_engine->stageHeight() / 2.0 - m_dragDeltaY);
    }
}

bool RenderedTarget::convexHullPointsNeeded() const
{
    return m_convexHullDirty || m_hullPoints.empty();
}

void RenderedTarget::updateHullPoints()
{
    m_convexHullDirty = false;

    if (!isVisible()) {
        m_hullPoints.clear();
        return;
    }

    m_hullPoints = textureManager()->getTextureConvexHullPoints(m_cpuTexture);
    // TODO: Apply graphic effects (#117)
}

bool RenderedTarget::containsLocalPoint(const QPointF &point) const
{
    if (!boundingRect().contains(point))
        return false;

    const std::vector<QPoint> &points = hullPoints();
    QPoint intPoint = point.toPoint();
    auto it = std::lower_bound(points.begin(), points.end(), intPoint, [](const QPointF &lhs, const QPointF &rhs) { return (lhs.y() < rhs.y()) || (lhs.y() == rhs.y() && lhs.x() < rhs.x()); });

    if (it == points.end()) {
        // The point is beyond the last point in the convex hull
        return false;
    }

    // Check if the point is equal to the one found
    return *it == intPoint;
}

QPointF RenderedTarget::transformPoint(double scratchX, double scratchY, double originX, double originY, double rot) const
{
    return transformPoint(scratchX, scratchY, originX, originY, std::sin(rot), std::cos(rot));
}

QPointF RenderedTarget::transformPoint(double scratchX, double scratchY, double originX, double originY, double sinRot, double cosRot) const
{
    const double x = (scratchX - originX) * cosRot - (scratchY - originY) * sinRot;
    const double y = (scratchX - originX) * sinRot + (scratchY - originY) * cosRot;
    return QPointF(x, y);
}

QPointF RenderedTarget::mapFromStageWithOriginPoint(const QPointF &scenePoint) const
{
    // mapFromItem() doesn't use the transformOriginPoint property, so we must do this ourselves
    QTransform t;
    const double mirror = m_mirrorHorizontally ? -1 : 1;
    const double originX = transformOriginPoint().x();
    const double originY = transformOriginPoint().y();
    t.translate(originX, originY);
    t.rotate(-rotation());
    t.scale(1 / scale() * mirror, 1 / scale());
    t.translate(-originX * mirror, -originY);
    t.translate(-x(), -y());

    QPointF localPoint = t.map(scenePoint);
    return localPoint;
}

QPointF RenderedTarget::mapFromScratchToLocal(const QPointF &point) const
{
    QTransform t;
    const double textureScale = m_skin->getTextureScale(m_cpuTexture);
    const double scale = m_size / textureScale;
    const double mirror = m_mirrorHorizontally ? -1 : 1;
    const double bitmapRes = m_costume->bitmapResolution();
    t.translate(m_costume->rotationCenterX() * textureScale, m_costume->rotationCenterY() * textureScale);
    t.rotate(-rotation());
    t.scale(bitmapRes * mirror / scale, -bitmapRes / scale);
    t.translate(-m_x, -m_y);

    QPointF localPoint = t.map(point);
    return localPoint;
}

CpuTextureManager *RenderedTarget::textureManager() const
{
    if (!m_textureManager)
        m_textureManager = std::make_shared<CpuTextureManager>();

    return m_textureManager.get();
}

void RenderedTarget::getVisibleTargets(std::vector<Target *> &dst) const
{
    dst.clear();

    if (!m_engine)
        return;

    const auto &targets = m_engine->targets();

    for (auto target : targets) {
        Q_ASSERT(target);

        if (target->isStage())
            dst.push_back(target.get());
        else {
            Sprite *sprite = static_cast<Sprite *>(target.get());

            if (sprite->visible())
                dst.push_back(target.get());

            const auto &clones = sprite->clones();

            for (auto clone : clones) {
                if (clone->visible())
                    dst.push_back(clone.get());
            }
        }
    }

    std::sort(dst.begin(), dst.end(), [](Target *t1, Target *t2) { return t1->layerOrder() > t2->layerOrder(); });
}

QRectF RenderedTarget::touchingBounds() const
{
    // https://github.com/scratchfoundation/scratch-render/blob/0a04c2fb165f5c20406ec34ab2ea5682ae45d6e0/src/RenderWebGL.js#L1330-L1350
    if (!m_engine)
        return QRectF();

    Rect scratchBounds = getFastBounds();

    // Limit queries to the stage size
    const double stageWidth = m_engine->stageWidth();
    const double stageHeight = m_engine->stageHeight();
    clampRect(scratchBounds, -stageWidth / 2, stageWidth / 2, -stageHeight / 2, stageHeight / 2);

    // TODO: Use Rect::snapToInt()
    QRect bounds(QPoint(scratchBounds.left(), scratchBounds.bottom()), QPoint(scratchBounds.right(), scratchBounds.top()));
    return bounds;
}

QRectF RenderedTarget::candidatesBounds(const QRectF &targetRect, const std::vector<Target *> &candidates, std::vector<IRenderedTarget *> &dst) const
{
    QRectF united;
    dst.clear();

    for (auto candidate : candidates) {
        Q_ASSERT(candidate);

        if (!candidate)
            continue;

        IRenderedTarget *target = nullptr;

        if (candidate->isStage()) {
            Stage *stage = static_cast<Stage *>(candidate);
            StageModel *model = static_cast<StageModel *>(stage->getInterface());
            Q_ASSERT(model);

            if (model)
                target = model->renderedTarget();
        } else {
            Sprite *sprite = static_cast<Sprite *>(candidate);
            SpriteModel *model = static_cast<SpriteModel *>(sprite->getInterface());
            Q_ASSERT(model);

            if (model)
                target = model->renderedTarget();
        }

        Q_ASSERT(target);

        if (target && target != this) {
            united = united.united(candidateIntersection(targetRect, target));
            dst.push_back(target);
        }
    }

    // Check pen layer
    if (m_penLayer)
        united = united.united(rectIntersection(targetRect, m_penLayer->getBounds()));

    return united;
}

QRectF RenderedTarget::candidatesBounds(const QRectF &targetRect, const std::vector<libscratchcpp::Sprite *> &candidates, std::vector<IRenderedTarget *> &dst) const
{
    QRectF united;
    dst.clear();

    for (auto candidate : candidates) {
        Q_ASSERT(candidate);

        if (!candidate)
            continue;

        IRenderedTarget *target = nullptr;
        SpriteModel *model = static_cast<SpriteModel *>(candidate->getInterface());
        Q_ASSERT(model);

        if (model)
            target = model->renderedTarget();

        Q_ASSERT(target);

        if (target && target != this) {
            united = united.united(candidateIntersection(targetRect, target));
            dst.push_back(target);
        }
    }

    return united;
}

QRectF RenderedTarget::candidateIntersection(const QRectF &targetRect, IRenderedTarget *target)
{
    Q_ASSERT(target);

    if (target) {
        // Calculate the intersection of the bounding rectangles
        Rect scratchRect = target->getFastBounds();
        return rectIntersection(targetRect, scratchRect);
    }

    return QRectF();
}

QRectF RenderedTarget::rectIntersection(const QRectF &targetRect, const Rect &candidateRect)
{
    // TODO: Use Rect::snapToInt()
    QRect rect(QPoint(candidateRect.left(), candidateRect.bottom()), QPoint(candidateRect.right(), candidateRect.top()));
    return targetRect.intersected(rect);
}

void RenderedTarget::clampRect(Rect &rect, double left, double right, double bottom, double top)
{
    // TODO: Use Rect::clamp()
    rect.setLeft(std::max(rect.left(), left));
    rect.setRight(std::min(rect.right(), right));
    rect.setBottom(std::max(rect.bottom(), bottom));
    rect.setTop(std::min(rect.top(), top));

    rect.setLeft(std::min(rect.left(), right));
    rect.setRight(std::max(rect.right(), left));
    rect.setBottom(std::min(rect.bottom(), top));
    rect.setTop(std::max(rect.top(), bottom));
}

QRgb RenderedTarget::convertColor(const libscratchcpp::Value &color)
{
    // TODO: Remove this after libscratchcpp starts converting colors (it still needs to be converted to RGB here)
    std::string stringValue;

    if (color.isString())
        stringValue = color.toString();

    if (!stringValue.empty() && stringValue[0] == '#') {
        bool valid = false;
        QColor color;

        if (stringValue.size() <= 7) // #RRGGBB
        {
            color = QColor::fromString(stringValue);
            valid = color.isValid();
        }

        if (!valid)
            color = Qt::black;

        return color.rgb();

    } else
        return QColor::fromRgba(static_cast<QRgb>(color.toLong())).rgb();
}

bool RenderedTarget::colorMatches(QRgb a, QRgb b)
{
    // https://github.com/scratchfoundation/scratch-render/blob/0a04c2fb165f5c20406ec34ab2ea5682ae45d6e0/src/RenderWebGL.js#L77-L81
    return (qRed(a) & 0b11111000) == (qRed(b) & 0b11111000) && (qGreen(a) & 0b11111000) == (qGreen(b) & 0b11111000) && (qBlue(a) & 0b11110000) == (qBlue(b) & 0b11110000);
}

QRgb RenderedTarget::sampleColor3b(double x, double y, const std::vector<IRenderedTarget *> &targets) const
{
    // https://github.com/scratchfoundation/scratch-render/blob/0a04c2fb165f5c20406ec34ab2ea5682ae45d6e0/src/RenderWebGL.js#L1966-L1990
    double blendAlpha = 1;
    QRgb blendColor;
    int r = 0, g = 0, b = 0;
    bool penLayerChecked = false;

    for (int i = 0; blendAlpha != 0 && i <= targets.size(); i++) { // NOTE: <= instead of < to process the pen layer
        Q_ASSERT(i == targets.size() || targets[i]);

        if ((i == targets.size() || targets[i]->stageModel()) && !penLayerChecked) {
            if (m_penLayer)
                blendColor = m_penLayer->colorAtScratchPoint(x, y);
            else
                blendColor = qRgba(0, 0, 0, 0);

            penLayerChecked = true;

            if (i < targets.size())
                i--; // check stage on next iteration
        } else if (i == targets.size())
            break;
        else
            blendColor = targets[i]->colorAtScratchPoint(x, y);

        r += qRed(blendColor) * blendAlpha;
        g += qGreen(blendColor) * blendAlpha;
        b += qBlue(blendColor) * blendAlpha;
        blendAlpha *= (1.0 - (qAlpha(blendColor) / 255.0));
    }

    r += blendAlpha * 255;
    g += blendAlpha * 255;
    b += blendAlpha * 255;
    return qRgb(r, g, b);
}

bool RenderedTarget::mirrorHorizontally() const
{
    return m_mirrorHorizontally;
}
