#include "errorParser.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QCoreApplication>

Errors ErrorParser::parse(QByteArray body) {
    Errors err{};
    QJsonDocument document = QJsonDocument::fromJson(body);
    if (document.isNull()) {
        err.errorMessage = QCoreApplication::translate("ServerLocale", "Server error");
        return err;
    }

    QJsonObject obj = document.object();
    err.errorMessage = obj["error"].toString();
    if (obj.contains("errors")) {
        QJsonObject errors = obj["errors"].toObject();

        if (errors.contains("fieldErrors")) {
            QJsonObject fieldErrors = errors["fieldErrors"].toObject();

            for (QString key : fieldErrors.keys()) {
                QJsonArray keyErrors = fieldErrors[key].toArray();
                QString keyErrorsJoined{};

                for (qsizetype i = 0; i < keyErrors.size(); i++) {
                    keyErrorsJoined += keyErrors[i].toString();
                    if (i != 0) keyErrorsJoined += "\n";
                }

                err.fieldErrors.insert(key, keyErrorsJoined);
            }
        }
    }

    return err;
}
