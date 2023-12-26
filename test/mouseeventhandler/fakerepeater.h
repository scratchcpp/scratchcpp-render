#pragma once

#include <QQuickItem>

namespace scratchcpprender
{

class FakeRepeater : public QQuickItem
{
        Q_OBJECT
        Q_PROPERTY(int count READ count)
    public:
        FakeRepeater(QQuickItem *parent = nullptr);

        int count() const;
        Q_INVOKABLE QQuickItem *itemAt(int index) const;

        std::vector<QQuickItem *> items;
};

} // namespace scratchcpprender
