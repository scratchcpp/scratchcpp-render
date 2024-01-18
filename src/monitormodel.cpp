// SPDX-License-Identifier: LGPL-3.0-or-later

#include <scratchcpp/monitor.h>
#include <scratchcpp/sprite.h>

#include "monitormodel.h"

using namespace scratchcpprender;

MonitorModel::MonitorModel(QObject *parent) :
    QObject(parent)
{
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
        return m_monitor->visible();
    else
        return false;
}

void MonitorModel::init(libscratchcpp::Monitor *monitor)
{
    m_monitor = monitor;
}

void MonitorModel::onVisibleChanged(bool visible)
{
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
