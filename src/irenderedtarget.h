// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QtOpenGL>
#include <qnanoquickitem.h>
#include <scratchcpp/sprite.h>

#include "shadermanager.h"

class QBuffer;
class QNanoPainter;
class QOpenGLContext;

namespace scratchcpprender
{

class StageModel;
class SpriteModel;
class SceneMouseArea;
class Texture;

class IRenderedTarget : public QNanoQuickItem
{
    public:
        IRenderedTarget(QQuickItem *parent = nullptr) :
            QNanoQuickItem(parent)
        {
        }

        virtual ~IRenderedTarget() { }

        virtual void updateVisibility(bool visible) = 0;
        virtual void updateX(double x) = 0;
        virtual void updateY(double y) = 0;
        virtual void updateSize(double size) = 0;
        virtual void updateDirection(double direction) = 0;
        virtual void updateRotationStyle(libscratchcpp::Sprite::RotationStyle style) = 0;
        virtual void updateLayerOrder(int layerOrder) = 0;
        virtual void updateCostume(libscratchcpp::Costume *costume) = 0;

        virtual bool costumesLoaded() const = 0;
        virtual void loadCostumes() = 0;

        virtual void beforeRedraw() = 0;

        virtual void deinitClone() = 0;

        virtual libscratchcpp::IEngine *engine() const = 0;
        virtual void setEngine(libscratchcpp::IEngine *newEngine) = 0;

        virtual StageModel *stageModel() const = 0;
        virtual void setStageModel(StageModel *newStageModel) = 0;

        virtual SpriteModel *spriteModel() const = 0;
        virtual void setSpriteModel(SpriteModel *newSpriteModel) = 0;

        virtual libscratchcpp::Target *scratchTarget() const = 0;

        virtual SceneMouseArea *mouseArea() const = 0;
        virtual void setMouseArea(SceneMouseArea *newMouseArea) = 0;

        virtual double stageScale() const = 0;
        virtual void setStageScale(double scale) = 0;

        virtual qreal width() const = 0;
        virtual void setWidth(qreal width) = 0;

        virtual qreal height() const = 0;
        virtual void setHeight(qreal width) = 0;

        virtual libscratchcpp::Rect getBounds() const = 0;
        virtual QRectF getBoundsForBubble() const = 0;
        virtual libscratchcpp::Rect getFastBounds() const = 0;

        virtual QPointF mapFromScene(const QPointF &point) const = 0;

        virtual bool mirrorHorizontally() const = 0;

        virtual Texture texture() const = 0;
        virtual const Texture &cpuTexture() const = 0;
        virtual int costumeWidth() const = 0;
        virtual int costumeHeight() const = 0;

        virtual const std::unordered_map<ShaderManager::Effect, double> &graphicEffects() const = 0;
        virtual void setGraphicEffect(ShaderManager::Effect effect, double value) = 0;
        virtual void clearGraphicEffects() = 0;

        virtual const std::vector<QPoint> &hullPoints() const = 0;

        virtual bool containsScratchPoint(double x, double y) const = 0;
        virtual QRgb colorAtScratchPoint(double x, double y) const = 0;

        virtual bool touchingClones(const std::vector<libscratchcpp::Sprite *> &clones) const = 0;
        virtual bool touchingColor(libscratchcpp::Rgb color) const = 0;
        virtual bool touchingColor(libscratchcpp::Rgb color, libscratchcpp::Rgb mask) const = 0;
};

} // namespace scratchcpprender
