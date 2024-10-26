#ifndef ERRORPARSER_H
#define ERRORPARSER_H

#include <QMap>
#include <QObject>

struct Errors {
  Q_GADGET

  Q_PROPERTY(QString errorMessage MEMBER errorMessage)
  Q_PROPERTY(QMap<QString, QString> fieldErrors MEMBER fieldErrors)

public:
  QString errorMessage;
  bool isFormError{};
  QMap<QString, QString> fieldErrors;
};

class ErrorParser {
public:
  static Errors parse(QByteArray body, bool isFormError = false);
};

#endif // ERRORPARSER_H
