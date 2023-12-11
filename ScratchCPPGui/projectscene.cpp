// SPDX-License-Identifier: LGPL-3.0-or-later

#include <scratchcpp/iengine.h>

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

void ProjectScene::handleMouseMove(qreal x, qreal y)
{
    if (m_engine) {
        m_engine->setMouseX(x - m_engine->stageWidth() / 2.0);
        m_engine->setMouseY(-y + m_engine->stageHeight() / 2.0);
    }
}

} // namespace scratchcppgui
