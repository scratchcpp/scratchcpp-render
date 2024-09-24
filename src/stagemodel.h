// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QObject>
#include <scratchcpp/istagehandler.h>

#include "textbubbleshape.h"

Q_MOC_INCLUDE("renderedtarget.h");

namespace scratchcpprender
{

class IRenderedTarget;

class StageModel
    : public QObject
    , public libscratchcpp::IStageHandler
{
        Q_OBJECT
        Q_PROPERTY(IRenderedTarget *renderedTarget READ renderedTarget WRITE setRenderedTarget NOTIFY renderedTargetChanged)
        Q_PROPERTY(TextBubbleShape::Type bubbleType READ bubbleType NOTIFY bubbleTypeChanged)
        Q_PROPERTY(QString bubbleText READ bubbleText NOTIFY bubbleTextChanged)
        Q_PROPERTY(int bubbleLayer READ bubbleLayer NOTIFY bubbleLayerChanged)

    public:
        explicit StageModel(QObject *parent = nullptr);

        void init(libscratchcpp::Stage *stage) override;

        void onCostumeChanged(libscratchcpp::Costume *costume) override;

        void onTempoChanged(int tempo) override;
        void onVideoStateChanged(libscratchcpp::Stage::VideoState videoState) override;
        void onVideoTransparencyChanged(int videoTransparency) override;

        void onGraphicsEffectChanged(libscratchcpp::IGraphicsEffect *effect, double value) override;
        void onGraphicsEffectsCleared() override;

        int costumeWidth() const override;
        int costumeHeight() const override;

        libscratchcpp::Rect boundingRect() const override;
        libscratchcpp::Rect fastBoundingRect() const override;

        bool touchingClones(const std::vector<libscratchcpp::Sprite *> &clones) const override;
        bool touchingPoint(double x, double y) const override;
        bool touchingColor(const libscratchcpp::Value &color) const override;
        bool touchingColor(const libscratchcpp::Value &color, const libscratchcpp::Value &mask) const override;

        Q_INVOKABLE void loadCostume();

        libscratchcpp::Stage *stage() const;

        IRenderedTarget *renderedTarget() const;
        void setRenderedTarget(IRenderedTarget *newRenderedTarget);

        const TextBubbleShape::Type &bubbleType() const;

        const QString &bubbleText() const;

        int bubbleLayer() const;

    signals:
        void renderedTargetChanged();
        void bubbleTypeChanged();
        void bubbleTextChanged();
        void bubbleLayerChanged();

    private:
        libscratchcpp::Stage *m_stage = nullptr;
        IRenderedTarget *m_renderedTarget = nullptr;
        TextBubbleShape::Type m_bubbleType = TextBubbleShape::Type::Say;
        QString m_bubbleText;
};

} // namespace scratchcpprender
