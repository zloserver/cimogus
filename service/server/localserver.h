#ifndef LOCALSERVER_H
#define LOCALSERVER_H

#include <QObject>
#include <QPointer>
#include <QProcess>
#include <QSet>
#include <QSharedPointer>
#include <QStringList>
#include <QVector>

#include "ipcserver.h"

#include "../../client/daemon/daemonlocalserver.h"

#ifdef Q_OS_WIN
#include "windows/daemon/windowsdaemon.h"
#endif

#ifdef Q_OS_LINUX
#include "linux/daemon/linuxdaemon.h"
#endif

#ifdef Q_OS_MAC
#include "macos/daemon/macosdaemon.h"
#endif

class QLocalServer;
class QLocalSocket;
class QProcess;

class LocalServer : public QObject {
  Q_OBJECT

public:
  explicit LocalServer(QObject *parent = nullptr);
  ~LocalServer();
  QSharedPointer<QLocalServer> m_server;
  IpcServer m_ipcServer;
  IpcProcessTun2Socks m_tun2socks;
  QRemoteObjectHost m_serverNode;
  quint64 m_connectionCount = 0;
  bool m_isRemotingEnabled = false;
#ifdef Q_OS_LINUX
  DaemonLocalServer server{qApp};
  LinuxDaemon daemon;
#endif
#ifdef Q_OS_WIN
  DaemonLocalServer server{qApp};
  WindowsDaemon daemon;
#endif
#ifdef Q_OS_MAC
  DaemonLocalServer server{qApp};
  MacOSDaemon daemon;
#endif
};

#endif // LOCALSERVER_H
