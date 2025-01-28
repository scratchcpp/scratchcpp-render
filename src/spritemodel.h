// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QObject>
#include <QQmlEngine>
#include <scratchcpp/ispritehandler.h>

#include "penstate.h"
#include "textbubbleshape.h"

Q_MOC_INCLUDE("renderedtarget.h");
Q_MOC_INCLUDE("ipenlayer.h");

namespace scratchcpprender
{

class IRenderedTarget;
class IPenLayer;

class SpriteModel
    : public QObject
    , public libscratchcpp::ISpriteHandler
{
        Q_OBJECT
        QML_ELEMENT
        Q_PROPERTY(IRenderedTarget *renderedTarget READ renderedTarget WRITE setRenderedTarget NOTIFY renderedTargetChanged)
        Q_PROPERTY(IPenLayer *penLayer READ penLayer WRITE setPenLayer NOTIFY penLayerChanged)
        Q_PROPERTY(TextBubbleShape::Type bubbleType READ bubbleType NOTIFY bubbleTypeChanged)
        Q_PROPERTY(QString bubbleText READ bubbleText NOTIFY bubbleTextChanged)
        Q_PROPERTY(int bubbleLayer READ bubbleLayer NOTIFY bubbleLayerChanged)

    public:
        SpriteModel(QObject *parent = nullptr);

        void init(libscratchcpp::Sprite *sprite) override;
        void deinitClone() override;

        void onCloned(libscratchcpp::Sprite *clone) override;

        void onCostumeChanged(libscratchcpp::Costume *costume) override;

        void onVisibleChanged(bool visible) override;
        void onXChanged(double x) override;
        void onYChanged(double y) override;
        void onMoved(double oldX, double oldY, double newX, double newY) override;
        void onSizeChanged(double size) override;
        void onDirectionChanged(double direction) override;
        void onRotationStyleChanged(libscratchcpp::Sprite::RotationStyle rotationStyle) override;
        void onLayerOrderChanged(int layerOrder) override;

        void onGraphicsEffectChanged(libscratchcpp::IGraphicsEffect *effect, double value) override;
        void onGraphicsEffectsCleared() override;

        int costumeWidth() const override;
        int costumeHeight() const override;

        libscratchcpp::Rect boundingRect() const override;
        libscratchcpp::Rect fastBoundingRect() const override;

        bool touchingClones(const std::vector<libscratchcpp::Sprite *> &clones) const override;
        bool touchingPoint(double x, double y) const override;
        bool touchingColor(libscratchcpp::Rgb color) const override;
        bool touchingColor(libscratchcpp::Rgb color, libscratchcpp::Rgb mask) const override;

        libscratchcpp::Sprite *sprite() const;

        IRenderedTarget *renderedTarget() const;
        void setRenderedTarget(IRenderedTarget *newRenderedTarget);

        IPenLayer *penLayer() const;
        void setPenLayer(IPenLayer *newPenLayer);

        PenState &penState();
        PenAttributes &penAttributes();

        bool penDown() const;
        void setPenDown(bool newPenDown);

        SpriteModel *cloneRoot() const;

        const TextBubbleShape::Type &bubbleType() const;

        const QString &bubbleText() const;

        int bubbleLayer() const;

    signals:
        void renderedTargetChanged();
        void penLayerChanged();
        void bubbleTypeChanged();
        void bubbleTextChanged();
        void cloned(SpriteModel *cloneModel);
        void cloneDeleted(SpriteModel *clone);
        void bubbleLayerChanged();

    private:
        libscratchcpp::Sprite *m_sprite = nullptr;
        IRenderedTarget *m_renderedTarget = nullptr;
        IPenLayer *m_penLayer = nullptr;
        PenState m_penState;
        SpriteModel *m_cloneRoot = nullptr;
        TextBubbleShape::Type m_bubbleType = TextBubbleShape::Type::Say;
        QString m_bubbleText;
};

} // namespace scratchcpprender
