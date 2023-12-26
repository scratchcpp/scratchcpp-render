#include "fakerepeater.h"

using namespace scratchcpprender;

FakeRepeater::FakeRepeater(QQuickItem *parent) :
    QQuickItem(parent)
{
}

int FakeRepeater::count() const
{
    return items.size();
}

QQuickItem *FakeRepeater::itemAt(int index) const
{
    Q_ASSERT(index >= 0 && index < items.size());
    return items[index];
}
