#include "authController.h"

#include <QNetworkRequest>
#include <QUrlQuery>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QCoreApplication>
#include <QDesktopServices>
#include <logger.h>

#include <endpoints.h>

static QString localizeDaysLeft(qint64 days) {
    qint64 lastDigit = days % 10;

    if (lastDigit == 1) return QCoreApplication::translate("DaysLocale", "%1 day left");
    if (lastDigit == 2 || lastDigit == 3 || lastDigit == 4) return QCoreApplication::translate("DaysLocale234", "%1 days left");
    return QCoreApplication::translate("DaysLocale", "%1 days left");
}

static QString localizeHoursLeft(qint64 hours) {
    qint64 lastDigit = hours % 10;

    if (lastDigit == 1) QCoreApplication::translate("DaysLocale", "%1 hour left");
    if (lastDigit == 2 || lastDigit == 3 || lastDigit == 4) return QCoreApplication::translate("DaysLocale234", "%1 hours left");
    return QCoreApplication::translate("DaysLocale", "%1 hours left");
}

QString UserInfo::localizeTimeLeft() const {
    if (this->timeLeft == -1) return QString(localizeDaysLeft(100)).arg("ඞ");

    double hoursLeft = static_cast<double>(this->timeLeft) / 3600000;
    qint64 daysLeft = 0;
    if (hoursLeft >= 24) {
        daysLeft = hoursLeft / 24;
    }

    if (daysLeft > 0) return QString(localizeDaysLeft(daysLeft)).arg(QString::number(daysLeft));
    if (hoursLeft < 1 && hoursLeft > 0) return QCoreApplication::translate("DaysLocale", "less than an hour left");

    qint64 roundedHoursLeft = qRound(hoursLeft);
    return QString(localizeHoursLeft(roundedHoursLeft)).arg(QString::number(roundedHoursLeft));
}

qint64 UserInfo::monthsAvailableToAdd() const {
    if (this->timeLeft == -1) return 0;

    double hoursLeft = static_cast<double>(this->timeLeft) / 3600000;
    double daysLeft = 0;
    if (hoursLeft >= 24) {
        daysLeft = hoursLeft / 24;
    }

    qint64 monthsLeft = qCeil(daysLeft / 30);
    if (monthsLeft >= 6) return 0;
    return qMin(6 - monthsLeft, 6);
}

AuthController::AuthController(std::shared_ptr<Settings> settings, QObject *parent)
    : QObject{parent}, m_qnam(new QNetworkAccessManager{this}), m_settings(settings)
{
    connect(m_settings.get(), &Settings::settingsCleared, this, [this]() {
        m_token = "";
        m_authenticated = false;
    });

    connect(this, &AuthController::errorOccurred, this, [this](Errors errors) {
        QVariantMap map{};
        for (auto [key, value] : errors.fieldErrors.asKeyValueRange()) {
            map.insert(key, value);
        }

        emit this->errorOccurredQml(errors.errorMessage, map);
    });

    m_token = m_settings->getUserToken();
    m_authenticated = !m_token.isEmpty();

    refreshToken();
    refreshUserInfo();
    refreshServers();
}

bool AuthController::isAuthenticated() {
    return m_authenticated;
}

QString AuthController::getToken() {
    return m_token;
}

void AuthController::setToken(const QString& token) {
    m_token = token;
    m_settings->setUserToken(token);
    emit tokenUpdated();
}

void AuthController::setUnauthenticated() {
    m_authenticated = false;
    setToken("");
}

void AuthController::refreshToken() {
    QNetworkRequest request(QUrl(API_ROOT + REFRESH_ENDPOINT));
    request.setRawHeader("User-Agent", "ZloVpn");
    request.setRawHeader("Authorization", QString("Bearer " + m_token).toUtf8());

    QNetworkReply* reply = m_qnam->post(request, nullptr);

    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray data = reply->readAll();
            QJsonDocument document = QJsonDocument::fromJson(data);

            QString token = document.object()["token"].toString();
            setToken(token);
            m_authenticated = true;
        }
        else {
            m_authenticated = false;
        }
    });

    emit loginSuccessfull();
}

void AuthController::login(const QString& login, const QString& password) {
    QJsonObject body{};
    body["login"] = login;
    body["password"] = password;

    QJsonDocument doc{body};
    QByteArray bytes = doc.toJson();

    QNetworkRequest request(QUrl(API_ROOT + LOGIN_ENDPOINT));
    request.setRawHeader("User-Agent", "ZloVpn");
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("Content-Length", QByteArray::number(bytes.size()));

    QNetworkReply* reply = m_qnam->post(request, bytes);

    connect(reply, &QNetworkReply::finished, [this, reply]() {
        QByteArray data = reply->readAll();
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument document = QJsonDocument::fromJson(data);

            QString token = document.object()["token"].toString();
            setToken(token);
            m_authenticated = true;

            emit loginSuccessfull();
        }
        else {
            auto errors = ErrorParser::parse(data);
            emit errorOccurred(errors);
        }
    });
}

void AuthController::registerUser(const QString& email, const QString& username, const QString& password) {
    QJsonObject body{};
    body["email"] = email;
    body["username"] = username;
    body["password"] = password;

    QJsonDocument doc{body};
    QByteArray bytes = doc.toJson();

    QNetworkRequest request(QUrl(API_ROOT + REGISTER_ENDPOINT));
    request.setRawHeader("User-Agent", "ZloVpn");
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("Content-Length", QByteArray::number(bytes.size()));

    QNetworkReply* reply = m_qnam->post(request, bytes);

    connect(reply, &QNetworkReply::finished, [this, reply]() {
        QByteArray data = reply->readAll();
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument document = QJsonDocument::fromJson(data);

            QString token = document.object()["token"].toString();
            setToken(token);
            m_authenticated = true;

            emit registerSuccessfull();
        }
        else {
            auto errors = ErrorParser::parse(data);
            emit errorOccurred(errors);
        }
    });
}

void AuthController::logout() {
    setUnauthenticated();
}

void AuthController::refreshUserInfo() {
    QNetworkRequest request(QUrl(API_ROOT + ME_ENDPOINT));
    request.setRawHeader("User-Agent", "ZloVpn");
    request.setRawHeader("Authorization", QString("Bearer " + m_token).toUtf8());

    QNetworkReply* reply = m_qnam->get(request);

    connect(reply, &QNetworkReply::finished, [this, reply]() {
        QByteArray data = reply->readAll();
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument document = QJsonDocument::fromJson(data);

            QJsonObject object = document.object();
            QJsonObject userObject = object["user"].toObject();

            UserInfo info{};
            info.username = userObject["username"].toString();
            info.timeLeft = userObject["timeLeft"].toInteger();

            m_userInfo = info;

            emit userInfoUpdated();
        }
        else {
            auto httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            if (httpStatus == 401) {
                setUnauthenticated();
            }

            auto errors = ErrorParser::parse(data);
            emit errorOccurred(errors);
        }
    });
}

static RegionInfo parseRegionInfo(QJsonObject obj) {
    return RegionInfo{
        .id = obj["id"].toString(),
        .countryCode = obj["countryCode"].toString(),
        .countryName = obj["countryName"].toString()
    };
}

void AuthController::refreshServers() {
    QNetworkRequest request(QUrl(API_ROOT + SERVERS_ENDPOINT));
    request.setRawHeader("User-Agent", "ZloVpn");
    request.setRawHeader("Authorization", QString("Bearer " + m_token).toUtf8());

    QNetworkReply* reply = m_qnam->get(request);

    connect(reply, &QNetworkReply::finished, [this, reply]() {
        QByteArray data = reply->readAll();
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument document = QJsonDocument::fromJson(data);

            QJsonArray serverArray = document.array();

            QList<RegionInfo> servers{};

            for (QJsonValue value : serverArray) {
                QJsonObject serverObject = value.toObject();

                RegionInfo info = parseRegionInfo(serverObject);
                servers.append(info);
            }

            m_regions = servers;

            emit regionsUpdated();
        }
        else {
            auto httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            if (httpStatus == 401) {
                setUnauthenticated();
            }

            auto errors = ErrorParser::parse(data);
            emit errorOccurred(errors);
        }
    });
}

QString AuthController::getSelectedRegionId() {
    QString regionId = m_settings->getSelectedRegionId();
    if (regionId.isEmpty() && !m_regions.isEmpty()) {
        return m_regions[0].id;
    }

    return regionId;
}

void AuthController::setSelectedRegionId(QString regionId) {
    m_settings->setSelectedRegionId(regionId);
}

void AuthController::getServerConnectionString(const QString& serverId, ServerStringRequest* stringRequest) {
    QUrl url(API_ROOT + CONNECTION_STRING_ENDPOINT + "/" + serverId);
    QUrlQuery query{};
    query.addQueryItem("port", "10833");
    url.setQuery(query.query());

    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", "ZloVpn");
    request.setRawHeader("Authorization", QString("Bearer " + m_token).toUtf8());

    QNetworkReply* reply = m_qnam->get(request);

    connect(reply, &QNetworkReply::finished, stringRequest, [this, reply, stringRequest]() {
        QByteArray data = reply->readAll();
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument document = QJsonDocument::fromJson(data);

            QString connectionUrl = document.object()["url"].toString();
            emit stringRequest->stringArrived(connectionUrl);
        }
        else {
            auto httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            if (httpStatus == 401) {
                setUnauthenticated();
            }

            auto errors = ErrorParser::parse(data);
            emit stringRequest->errorOccurred(errors);
        }
    });
}

void AuthController::addBalance(qint32 months) {
    QJsonObject body{};
    body["months"] = months;

    QJsonDocument doc{ body };
    QByteArray bytes = doc.toJson();

    QNetworkRequest request(QUrl(API_ROOT + PAYMENT_ENDPOINT));
    request.setRawHeader("User-Agent", "ZloVpn");
    request.setRawHeader("Authorization", QString("Bearer " + m_token).toUtf8());

    QNetworkReply* reply = m_qnam->post(request, bytes);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        QByteArray data = reply->readAll();
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument document = QJsonDocument::fromJson(data);

            QString paymentUrl = document.object()["paymentUrl"].toString();
            if (!QDesktopServices::openUrl(paymentUrl)) {
                Errors errors{};
                errors.errorMessage = tr("Payment", "Failed to open payment link");
                emit errorOccurred(errors);
            }
            else {
                emit addBalanceOpened();
            }
        }
        else {
            auto httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            if (httpStatus == 401) {
                setUnauthenticated();
            }

            auto errors = ErrorParser::parse(data);
            emit errorOccurred(errors);
        }
    });
}
