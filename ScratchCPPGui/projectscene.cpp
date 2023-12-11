// SPDX-License-Identifier: LGPL-3.0-or-later

#include "projectscene.h"

namespace scratchcppgui
{

ProjectScene::ProjectScene(QQuickItem *parent)
{
}

libscratchcpp::IEngine *ProjectScene::engine() const
{
    return m_engine;
}

void ProjectScene::setEngine(libscratchcpp::IEngine *newEngine)
{
    if (m_engine == newEngine)
        return;

    m_engine = newEngine;
    emit engineChanged();
}

} // namespace scratchcppgui
