#pragma once

#include <QObject>

#ifdef Q_OS_MACOS
#include <AmneziaVPN-Swift.h>
#endif

#include <optional>

class AutoUpdateController : public QObject {
  Q_OBJECT
  Q_PROPERTY(bool supportsAutoUpdates READ supportsAutoUpdates)
public:
  explicit AutoUpdateController(QObject *parent = nullptr);

public slots:
  void init(QString spikeUrl);
  void checkForUpdates();
  bool canCheckForUpdates();
  bool supportsAutoUpdates();

private:
#ifdef Q_OS_MACOS
  std::optional<ZloVPN::MacAutoUpdater> m_macAutoUpdater{};
#endif
};
