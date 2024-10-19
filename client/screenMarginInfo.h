#pragma once

#include <QObject>

struct ScreenMarginInfo
{
    Q_GADGET

    Q_PROPERTY(double top MEMBER top)
    Q_PROPERTY(double bottom MEMBER bottom)
    Q_PROPERTY(double left MEMBER left)
    Q_PROPERTY(double right MEMBER right)

public:
    bool operator==(const ScreenMarginInfo &other) const
    {
        return top == other.top && bottom == other.bottom && left == other.left && right == other.right;
    }

    bool operator!=(const ScreenMarginInfo &other) const
    {
        return !(*this == other);
    }

    double top{};
    double bottom{};
    double left{};
    double right{};
};

class ScreenMarginController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ScreenMarginInfo margins MEMBER m_margins NOTIFY marginsUpdated);
    
public:
    explicit ScreenMarginController(QObject *parent = nullptr);
    
public slots:
    void refreshScreenMargins();
    
signals:
    void marginsUpdated();
    
private:
    ScreenMarginInfo m_margins;
};
