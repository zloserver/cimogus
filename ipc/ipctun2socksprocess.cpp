#include "ipctun2socksprocess.h"
#include "ipc.h"
#include <QProcess>
#include <QString>

#include "../protocols/protocols_defs.h"

#ifndef Q_OS_IOS

IpcProcessTun2Socks::IpcProcessTun2Socks(QObject *parent) :
    IpcProcessTun2SocksSource(parent),
    m_t2sProcess(QSharedPointer<QProcess>(new QProcess()))
{
    connect(m_t2sProcess.data(), &QProcess::stateChanged, this, &IpcProcessTun2Socks::stateChanged);
    qDebug() << "IpcProcessTun2Socks::IpcProcessTun2Socks()";

}

IpcProcessTun2Socks::~IpcProcessTun2Socks()
{
    qDebug() << "IpcProcessTun2Socks::~IpcProcessTun2Socks()";
}

void IpcProcessTun2Socks::start()
{
    qDebug() << "IpcProcessTun2Socks::start()";
    m_t2sProcess->setProgram(amnezia::permittedProcessPath(static_cast<amnezia::PermittedProcess>(amnezia::PermittedProcess::Tun2Socks)));
    QString XrayConStr = "socks5://127.0.0.1:10833";

#ifdef Q_OS_WIN
    QStringList arguments({"-device", "tun://tun3", "-proxy", XrayConStr, "-tun-post-up",
                           QString("cmd /c netsh interface ip set address name=\"tun3\" static %1 255.255.255.255")
                               .arg(amnezia::protocols::xray::defaultLocalAddr)});
#endif
#ifdef Q_OS_LINUX
    QStringList arguments({"-device", "tun://tun3", "-proxy", XrayConStr});
#endif
#ifdef Q_OS_MAC
    QStringList arguments({"-device", "utun22", "-proxy", XrayConStr});
#endif

    m_t2sProcess->setArguments(arguments);

    Utils::killProcessByName(m_t2sProcess->program());

    connect(m_t2sProcess.data(), &QProcess::readyReadStandardOutput, this, &IpcProcessTun2Socks::bingus);
    connect(m_t2sProcess.data(), &QProcess::readyReadStandardError, this, [this]() {
        QString line = m_t2sProcess.data()->readAllStandardError();
        qDebug() << line;
    });

    connect(m_t2sProcess.data(), QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &IpcProcessTun2Socks::bungus);

    m_t2sProcess->start();

    m_t2sProcess->start();
    m_t2sProcess->waitForStarted();
}

void IpcProcessTun2Socks::bingus() {
    QString line = m_t2sProcess.data()->readAllStandardOutput();
    qDebug().noquote() << "tun2socks: " << line;
    if (line.contains("[STACK] tun://") && line.contains("<-> socks5://127.0.0.1")) {
        emit setConnectionState(Vpn::ConnectionState::Connected);
    }
}

void IpcProcessTun2Socks::bungus(int exitCode, QProcess::ExitStatus exitStatus) {
    qDebug().noquote() << "tun2socks finished, exitCode, exiStatus" << exitCode << exitStatus;
    emit setConnectionState(Vpn::ConnectionState::Disconnected);
    if (exitStatus != QProcess::NormalExit) {
        stop();
    }
    if (exitCode != 0) {
        stop();
    }
}

void IpcProcessTun2Socks::stop()
{
    qDebug() << "IpcProcessTun2Socks::stop()";
    m_t2sProcess->close();
}
#endif
