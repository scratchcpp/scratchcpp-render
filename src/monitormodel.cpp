// SPDX-License-Identifier: LGPL-3.0-or-later

#include <scratchcpp/monitor.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/iextension.h>

#include "monitormodel.h"

using namespace scratchcpprender;

MonitorModel::MonitorModel(QObject *parent) :
    MonitorModel(nullptr, parent)
{
}

MonitorModel::MonitorModel(libscratchcpp::IExtension *extension, QObject *parent) :
    QObject(parent)
{
    if (!extension)
        return;

    // TODO: Get the color from the extension
    std::string name = extension->name();
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

QString MonitorModel::name() const
{
    if (m_monitor) {
        if (libscratchcpp::Sprite *sprite = m_monitor->sprite())
            return QString::fromStdString(sprite->name() + ": " + m_monitor->name());
        else
            return QString::fromStdString(m_monitor->name());
    } else
        return "";
}

bool MonitorModel::visible() const
{
    if (m_monitor)
        return m_monitor->visible() && !m_monitor->needsAutoPosition();
    else
        return false;
}

const QColor &MonitorModel::color() const
{
    return m_color;
}

void MonitorModel::init(libscratchcpp::Monitor *monitor)
{
    m_monitor = monitor;
}

void MonitorModel::onVisibleChanged(bool visible)
{
    emit visibleChanged();
}

void MonitorModel::onXChanged(int x)
{
    emit xChanged();
    emit visibleChanged();
}

void MonitorModel::onYChanged(int y)
{
    emit yChanged();
    emit visibleChanged();
}

libscratchcpp::Monitor *MonitorModel::monitor() const
{
    return m_monitor;
}

int MonitorModel::x() const
{
    if (m_monitor)
        return m_monitor->x();
    else
        return 0;
}

int MonitorModel::y() const
{
    if (m_monitor)
        return m_monitor->y();
    else
        return 0;
}

unsigned int MonitorModel::width() const
{
    if (m_monitor)
        return m_monitor->width();
    else
        return 0;
}

void MonitorModel::setWidth(unsigned int newWidth)
{
    if (m_monitor) {
        m_monitor->setWidth(newWidth);
        emit widthChanged();
    }
}

unsigned int MonitorModel::height() const
{
    if (m_monitor)
        return m_monitor->height();
    else
        return 0;
}

void MonitorModel::setHeight(unsigned int newHeight)
{
    if (m_monitor) {
        m_monitor->setHeight(newHeight);
        emit heightChanged();
    }
}
