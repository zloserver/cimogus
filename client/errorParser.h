#ifndef ERRORPARSER_H
#define ERRORPARSER_H

#include <QObject>
#include <QMap>

struct Errors {
    Q_GADGET

    Q_PROPERTY(QString errorMessage MEMBER errorMessage)
    Q_PROPERTY(QMap<QString, QString> fieldErrors MEMBER fieldErrors)

public:

    QString errorMessage;
    QMap<QString, QString> fieldErrors;
};

class ErrorParser
{
public:
    static Errors parse(QByteArray body);
};

#endif // ERRORPARSER_H
