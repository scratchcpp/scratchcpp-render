// SPDX-License-Identifier: LGPL-3.0-or-later

#include <scratchcpp/virtualmachine.h>
#include <scratchcpp/iblocksection.h>
#include <scratchcpp/monitor.h>

#include "valuemonitormodel.h"

using namespace scratchcpprender;
using namespace libscratchcpp;

static const std::unordered_map<Monitor::Mode, ValueMonitorModel::Mode>
    MODE_MAP = { { Monitor::Mode::Default, ValueMonitorModel::Mode::Default }, { Monitor::Mode::Large, ValueMonitorModel::Mode::Large }, { Monitor::Mode::Slider, ValueMonitorModel::Mode::Slider } };

ValueMonitorModel::ValueMonitorModel(QObject *parent) :
    MonitorModel(parent)
{
}

ValueMonitorModel::ValueMonitorModel(IBlockSection *section, QObject *parent) :
    MonitorModel(parent)
{
    if (!section)
        return;

    // TODO: Get the color from the block section
    std::string name = section->name();
    if (name == "Motion")
        m_color = QColor::fromString("#4C97FF");
    else if (name == "Looks")
        m_color = QColor::fromString("#9966FF");
    else if (name == "Sound")
        m_color = QColor::fromString("#CF63CF");
    else if (name == "Sensing")
        m_color = QColor::fromString("#5CB1D6");
    else if (name == "Variables")
        m_color = QColor::fromString("#FF8C1A");
    else if (name == "Lists")
        m_color = QColor::fromString("#FF661A");
}

void ValueMonitorModel::onValueChanged(const VirtualMachine *vm)
{
    if (vm->registerCount() == 1) {
        m_value = QString::fromStdString(vm->getInput(0, 1)->toString());
        emit valueChanged();
    }
}

MonitorModel::Type ValueMonitorModel::type() const
{
    return Type::Value;
}

const QString &ValueMonitorModel::value() const
{
    return m_value;
}

void ValueMonitorModel::setValue(const QString &newValue)
{
    if (newValue == m_value)
        return;

    if (monitor())
        monitor()->changeValue(newValue.toStdString());
    else {
        m_value = newValue;
        emit valueChanged();
    }
}

const QColor &ValueMonitorModel::color() const
{
    return m_color;
}

ValueMonitorModel::Mode ValueMonitorModel::mode() const
{
    if (monitor())
        return MODE_MAP.at(monitor()->mode());
    else
        return Mode::Default;
}

double ValueMonitorModel::sliderMin() const
{
    if (monitor())
        return monitor()->sliderMin();
    else
        return 0;
}

double ValueMonitorModel::sliderMax() const
{
    if (monitor())
        return monitor()->sliderMax();
    else
        return 0;
}

bool ValueMonitorModel::discrete() const
{
    if (monitor())
        return monitor()->discrete();
    else
        return true;
}
