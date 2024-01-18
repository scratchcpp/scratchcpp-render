// SPDX-License-Identifier: LGPL-3.0-or-later

#include <scratchcpp/iblocksection.h>
#include <scratchcpp/virtualmachine.h>

#include "listmonitormodel.h"
#include "listmonitorlistmodel.h"

using namespace scratchcpprender;

ListMonitorModel::ListMonitorModel(QObject *parent) :
    MonitorModel(parent)
{
    m_listModel = new ListMonitorListModel(this);
}

ListMonitorModel::ListMonitorModel(libscratchcpp::IBlockSection *section, QObject *parent) :
    ListMonitorModel(parent)
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

void ListMonitorModel::onValueChanged(const libscratchcpp::VirtualMachine *vm)
{
    if (vm->registerCount() == 1) {
        long index = vm->getInput(0, 1)->toLong();
        libscratchcpp::List *list = vm->lists()[index];
        m_listModel->setList(list);
    }
}

MonitorModel::Type ListMonitorModel::type() const
{
    return Type::List;
}

const QColor &ListMonitorModel::color() const
{
    return m_color;
}

ListMonitorListModel *ListMonitorModel::listModel() const
{
    return m_listModel;
}
