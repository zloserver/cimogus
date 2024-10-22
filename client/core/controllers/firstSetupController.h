#ifndef FIRSTSETUPCONTROLLER_H
#define FIRSTSETUPCONTROLLER_H

#include <QObject>
#include <QString>

class FirstSetupController : public QObject {
  Q_OBJECT

public:
  explicit FirstSetupController(QObject *parent = nullptr);

public slots:
  bool firstSetupNeeded();
  void doFirstSetup();
  void restartService();

signals:
  void firstSetupFinished();
  void firstSetupFailed(bool requiresApproval, QString string);
};

#endif // FIRSTSETUPCONTROLLER_H
