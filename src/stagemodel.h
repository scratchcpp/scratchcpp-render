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

    public:
        explicit StageModel(QObject *parent = nullptr);

        void init(libscratchcpp::Stage *stage) override;

        void onCostumeChanged(libscratchcpp::Costume *costume) override;

        void onTempoChanged(int tempo) override;
        void onVideoStateChanged(libscratchcpp::Stage::VideoState videoState) override;
        void onVideoTransparencyChanged(int videoTransparency) override;

        void onGraphicsEffectChanged(libscratchcpp::IGraphicsEffect *effect, double value) override;
        void onGraphicsEffectsCleared() override;

        void onBubbleTypeChanged(libscratchcpp::Target::BubbleType type) override;
        void onBubbleTextChanged(const std::string &text) override;

        Q_INVOKABLE void loadCostume();

        libscratchcpp::Stage *stage() const;

        IRenderedTarget *renderedTarget() const;
        void setRenderedTarget(IRenderedTarget *newRenderedTarget);

        const TextBubbleShape::Type &bubbleType() const;

        const QString &bubbleText() const;

    signals:
        void renderedTargetChanged();
        void bubbleTypeChanged();
        void bubbleTextChanged();

    private:
        libscratchcpp::Stage *m_stage = nullptr;
        IRenderedTarget *m_renderedTarget = nullptr;
        TextBubbleShape::Type m_bubbleType = TextBubbleShape::Type::Say;
        QString m_bubbleText;
};

} // namespace scratchcpprender
