#include "screenMarginInfo.h"

#ifdef Q_OS_IOS
#include <AmneziaVPN-Swift.h>
#endif

ScreenMarginController::ScreenMarginController(QObject *parent)
    : QObject{parent}
{
}

void ScreenMarginController::refreshScreenMargins() {
#ifdef Q_OS_IOS
    ZloVPN::ScreenMargins margins = ZloVPN::getScreenMargins();
    m_margins = ScreenMarginInfo{.top = margins.getTopMargin(), .bottom = margins.getBottomMargin(), .left = margins.getLeftMargin(), .right = margins.getRightMargin()};
#else
    m_margins = ScreenMarginInfo{.bottom = 0, .top = 0, .left = 0, .right = 0};
#endif
    
    emit marginsUpdated();
}
