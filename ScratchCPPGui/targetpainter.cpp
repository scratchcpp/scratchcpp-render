// SPDX-License-Identifier: LGPL-3.0-or-later

#include <scratchcpp/target.h>
#include <scratchcpp/costume.h>

#include "targetpainter.h"
#include "renderedtarget.h"
#include "spritemodel.h"

using namespace scratchcppgui;

TargetPainter::TargetPainter()
{
}

TargetPainter::~TargetPainter()
{
}

void TargetPainter::paint(QNanoPainter *painter)
{
    unsigned char *svgBitmap = m_target->svgBitmap();
    double width = m_target->width();
    double height = m_target->height();

    if (svgBitmap) {
        // TODO: Paint shapes from SVG directly instead of painting rasterized SVG
        for (int i = 0; i < width * height; ++i) {
            int pixelIndex = i * 4; // Each pixel has four values (R, G, B, A)

            uchar red = svgBitmap[pixelIndex];
            uchar green = svgBitmap[pixelIndex + 1];
            uchar blue = svgBitmap[pixelIndex + 2];
            uchar alpha = svgBitmap[pixelIndex + 3];

            int x = i % static_cast<int>(width);
            int y = i / static_cast<int>(height);

            painter->setFillStyle(QNanoColor(red, green, blue, alpha));
            painter->fillRect(x, y, 1, 1);
        }
    } else {
        QNanoImage image = QNanoImage::fromCache(painter, m_target->bitmapBuffer(), m_target->bitmapUniqueKey());
        painter->drawImage(image, 0, 0, width, height);
    }
}

void TargetPainter::synchronize(QNanoQuickItem *item)
{
    m_target = dynamic_cast<RenderedTarget *>(item);
    Q_ASSERT(m_target);
}
