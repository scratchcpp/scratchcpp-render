#include <scratchcpp-render/scratchcpp-render.h>
#include <QQuickWindow>

#include "../common.h"

TEST(GlobalFunctionsTest, Init)
{
    qputenv("QSG_RENDER_LOOP", "threaded");
    QQuickWindow::setGraphicsApi(QSGRendererInterface::Software);

    scratchcpprender::init();
    ASSERT_EQ(qgetenv("QSG_RENDER_LOOP"), "basic");
    ASSERT_EQ(QQuickWindow::graphicsApi(), QSGRendererInterface::OpenGL);
}
