// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <qnanoquickitem.h>

namespace scratchcpprender
{

class TextBubbleShape : public QNanoQuickItem
{
        Q_OBJECT
        QML_ELEMENT
        Q_PROPERTY(Type type READ type WRITE setType NOTIFY typeChanged)
        Q_PROPERTY(bool onSpriteRight READ onSpriteRight WRITE setOnSpriteRight NOTIFY onSpriteRightChanged)
        Q_PROPERTY(double stageScale READ stageScale WRITE setStageScale NOTIFY stageScaleChanged)
        Q_PROPERTY(double nativeWidth READ nativeWidth WRITE setNativeWidth NOTIFY nativeWidthChanged)
        Q_PROPERTY(double nativeHeight READ nativeHeight WRITE setNativeHeight NOTIFY nativeHeightChanged)

    public:
        enum class Type
        {
            Say,
            Think
        };

        Q_ENUM(Type)

        TextBubbleShape(QQuickItem *parent = nullptr);

        Type type() const;
        void setType(Type newType);

        bool onSpriteRight() const;
        void setOnSpriteRight(bool newOnSpriteRight);

        double stageScale() const;
        void setStageScale(double newStageScale);

        double nativeWidth() const;
        void setNativeWidth(double newNativeWidth);

        double nativeHeight() const;
        void setNativeHeight(double newNativeHeight);

    signals:
        void typeChanged();
        void onSpriteRightChanged();
        void stageScaleChanged();
        void nativeWidthChanged();
        void nativeHeightChanged();

    protected:
        QNanoQuickItemPainter *createItemPainter() const override;

    private:
        Type m_type = Type::Say;
        bool m_onSpriteRight = true;
        double m_stageScale = 1;
        double m_nativeWidth = 0;
        double m_nativeHeight = 0;
};

} // namespace scratchcpprender
