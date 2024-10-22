#include "firstSetupController.h"

#include <string>

#ifdef Q_OS_MACOS
#include <AmneziaVPN-Swift.h>
#endif

FirstSetupController::FirstSetupController(QObject *parent) : QObject{parent} {}

bool FirstSetupController::firstSetupNeeded() {
#ifdef Q_OS_MACOS
  return ZloVPN::firstSetupNeeded();
#endif

  return false;
}

void FirstSetupController::doFirstSetup() {
#ifdef Q_OS_MACOS
  ZloVPN::FirstSetupResponse response = ZloVPN::doFirstSetup();

  if (response.isError()) {
    std::string errorString = response.getErrorString();
    emit firstSetupFailed(response.getRequiresApproval(), QString::fromStdString(errorString));
    return;
  }
#endif

  emit firstSetupFinished();
}
  
void FirstSetupController::restartService() {
#ifdef Q_OS_MACOS
  ZloVPN::restartService();
#endif
}
