// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QObject>
#include <scratchcpp/sprite.h>

#include "penstate.h"
#include "textbubbleshape.h"

Q_MOC_INCLUDE("renderedtarget.h");
Q_MOC_INCLUDE("ipenlayer.h");

namespace scratchcpprender
{

class IRenderedTarget;
class IPenLayer;

class TargetModel : public QObject
{
        Q_OBJECT
        Q_PROPERTY(IRenderedTarget *renderedTarget READ renderedTarget WRITE setRenderedTarget NOTIFY renderedTargetChanged)
        Q_PROPERTY(IPenLayer *penLayer READ penLayer WRITE setPenLayer NOTIFY penLayerChanged)
        Q_PROPERTY(TextBubbleShape::Type bubbleType READ bubbleType NOTIFY bubbleTypeChanged)
        Q_PROPERTY(QString bubbleText READ bubbleText NOTIFY bubbleTextChanged)
        Q_PROPERTY(int bubbleLayer READ bubbleLayer NOTIFY bubbleLayerChanged)

    public:
        explicit TargetModel(QObject *parent = nullptr);

        IRenderedTarget *renderedTarget() const;
        void setRenderedTarget(IRenderedTarget *newRenderedTarget);

        IPenLayer *penLayer() const;
        void setPenLayer(IPenLayer *newPenLayer);

        PenState &penState();
        PenAttributes &penAttributes();

        bool penDown() const;
        void setPenDown(bool newPenDown);

        const TextBubbleShape::Type &bubbleType() const;

        const QString &bubbleText() const;

        virtual int bubbleLayer() const { return 0; }

        Q_INVOKABLE virtual void loadCostume() { }

    signals:
        void renderedTargetChanged();
        void penLayerChanged();
        void bubbleTypeChanged();
        void bubbleTextChanged();
        void bubbleLayerChanged();

    protected:
        void setupTextBubble(libscratchcpp::TextBubble *bubble);

        void updateVisibility(bool visible);
        void updateX(double x);
        void updateY(double y);
        void updateSize(double size);
        void updateDirection(double direction);
        void updateRotationStyle(libscratchcpp::Sprite::RotationStyle style);
        void updateLayerOrder(int layerOrder);
        void updateCostume(libscratchcpp::Costume *costume);

        void onMoved(double oldX, double oldY, double newX, double newY);

        void setGraphicEffect(libscratchcpp::IGraphicsEffect *effect, double value);
        void clearGraphicEffects();

        int costumeWidth() const;
        int costumeHeight() const;

        void getBoundingRect(libscratchcpp::Rect &dst) const;
        void getFastBoundingRect(libscratchcpp::Rect &dst) const;

        bool touchingClones(const std::vector<libscratchcpp::Sprite *> &clones) const;
        bool touchingPoint(double x, double y) const;
        bool touchingColor(libscratchcpp::Rgb color) const;
        bool touchingColor(libscratchcpp::Rgb color, libscratchcpp::Rgb mask) const;

        virtual void drawPenPoint(IPenLayer *penLayer, const PenAttributes &penAttributes) { }                                            // stage and sprites can draw points
        virtual void drawPenLine(IPenLayer *penLayer, const PenAttributes &penAttributes, double x0, double y0, double x1, double y1) { } // only sprites can draw lines

    private:
        IRenderedTarget *m_renderedTarget = nullptr;
        IPenLayer *m_penLayer = nullptr;
        PenState m_penState;
        TextBubbleShape::Type m_bubbleType = TextBubbleShape::Type::Say;
        QString m_bubbleText;
};

} // namespace scratchcpprender
