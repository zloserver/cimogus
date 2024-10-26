#include "autoUpdateController.h"

#include "endpoints.h"

AutoUpdateController::AutoUpdateController(QObject *parent) : QObject{parent} {}

void AutoUpdateController::init(QString spikeUrl) {
#ifdef Q_OS_MACOS
  QString feedUrl = spikeUrl + MAC_UPDATE_ENDPOINT;
  swift::String feedUrlSwift = feedUrl.toStdString();
  m_macAutoUpdater =
    ZloVPN::MacAutoUpdater::init(feedUrlSwift);
#endif
}

void AutoUpdateController::checkForUpdates() {
#ifdef Q_OS_MACOS
  if (m_macAutoUpdater) {
    m_macAutoUpdater->checkForUpdates();
  }
#endif
}

bool AutoUpdateController::canCheckForUpdates() {
#ifdef Q_OS_MACOS
  if (!m_macAutoUpdater)
    return false;
  return m_macAutoUpdater->canCheckForUpdates();
#endif

  return false;
}

bool AutoUpdateController::supportsAutoUpdates() {
#ifdef Q_OS_MACOS
  return true;
#endif

  return false;
}
