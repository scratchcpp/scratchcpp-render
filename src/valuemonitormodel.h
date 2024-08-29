// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "monitormodel.h"

namespace scratchcpprender
{

class ValueMonitorModel : public MonitorModel
{
        Q_OBJECT
        QML_ELEMENT
        Q_PROPERTY(QString value READ value WRITE setValue NOTIFY valueChanged)
        Q_PROPERTY(Mode mode READ mode NOTIFY modeChanged)
        Q_PROPERTY(double sliderMin READ sliderMin NOTIFY sliderMinChanged)
        Q_PROPERTY(double sliderMax READ sliderMax NOTIFY sliderMaxChanged)
        Q_PROPERTY(bool discrete READ discrete NOTIFY discreteChanged)

    public:
        enum class Mode
        {
            Default,
            Large,
            Slider
        };

        Q_ENUM(Mode)

        ValueMonitorModel(QObject *parent = nullptr);
        ValueMonitorModel(libscratchcpp::IBlockSection *section, QObject *parent = nullptr);

        void onValueChanged(const libscratchcpp::VirtualMachine *vm) override;

        Type type() const override;

        const QString &value() const;
        void setValue(const QString &newValue);

        Mode mode() const;
        double sliderMin() const;
        double sliderMax() const;
        bool discrete() const;

    signals:
        void valueChanged();
        void colorChanged();
        void modeChanged();
        void sliderMinChanged();
        void sliderMaxChanged();
        void discreteChanged();

    private:
        QString m_value;
};

} // namespace scratchcpprender
