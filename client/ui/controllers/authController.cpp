#include "authController.h"

#include "core/server_defs.h"

#include <Cactus/Cactus.hpp>
#include <QCoreApplication>
#include <QDesktopServices>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrlQuery>
#include <logger.h>

#include <endpoints.h>

static QString localizeDaysLeft(qint64 days) {
  qint64 lastDigit = days % 10;

  if (lastDigit == 1)
    return QCoreApplication::translate("DaysLocale", "%1 day left");
  if (lastDigit == 2 || lastDigit == 3 || lastDigit == 4)
    return QCoreApplication::translate("DaysLocale234", "%1 days left");
  return QCoreApplication::translate("DaysLocale", "%1 days left");
}

static QString localizeHoursLeft(qint64 hours) {
  qint64 lastDigit = hours % 10;

  if (lastDigit == 1)
    QCoreApplication::translate("DaysLocale", "%1 hour left");
  if (lastDigit == 2 || lastDigit == 3 || lastDigit == 4)
    return QCoreApplication::translate("DaysLocale234", "%1 hours left");
  return QCoreApplication::translate("DaysLocale", "%1 hours left");
}

QString UserInfo::localizeTimeLeft() const {
  if (this->timeLeft == -1)
    return QString(localizeDaysLeft(100)).arg("ඞඞඞ");

  double hoursLeft = static_cast<double>(this->timeLeft) / 3600000;
  qint64 daysLeft = 0;
  if (hoursLeft >= 24) {
    daysLeft = hoursLeft / 24;
  }

  if (daysLeft > 0)
    return QString(localizeDaysLeft(daysLeft)).arg(QString::number(daysLeft));
  if (hoursLeft < 1 && hoursLeft > 0)
    return QCoreApplication::translate("DaysLocale", "less than an hour left");

  qint64 roundedHoursLeft = qRound(hoursLeft);
  return QString(localizeHoursLeft(roundedHoursLeft))
      .arg(QString::number(roundedHoursLeft));
}

qint64 UserInfo::monthsAvailableToAdd() const {
  if (this->timeLeft == -1)
    return 0;

  double hoursLeft = static_cast<double>(this->timeLeft) / 3600000;
  double daysLeft = 0;
  if (hoursLeft >= 24) {
    daysLeft = hoursLeft / 24;
  }

  qint64 monthsLeft = qCeil(daysLeft / 30);
  if (monthsLeft >= 6)
    return 0;
  return qMin(6 - monthsLeft, 6);
}

AuthController::AuthController(std::shared_ptr<Settings> settings,
                               QObject *parent)
    : QObject{parent}, m_qnam(new QNetworkAccessManager{this}),
      m_settings(settings) {
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

  connect(this, &AuthController::apiCompatibilityChanged, this, [this]() {
    if (!m_updateRequired)
      refreshToken();
  });

  m_token = m_settings->getUserToken();

  auto &cactus = ICactus::GetInstance();
  cactus.Spike(
      this,
      [this](QString spike) {
        m_spike = spike;
        m_spikeErrored = false;
        emit spikeUpdated();
        checkApiCompatibility();
      },
      [this]() {
        m_spikeErrored = true;
        emit spikeUpdated();
        emit spikeErrorOccurred();
      });
}

bool AuthController::isAuthenticated() { return m_authenticated; }

bool AuthController::hasToken() { return !getToken().isEmpty(); }

QString AuthController::getToken() { return m_token; }

void AuthController::setToken(const QString &token) {
  m_token = token;
  m_settings->setUserToken(token);

  bool previousState = m_authenticated;
  m_authenticated = !m_token.isEmpty();
  bool authenticationStateChanged = previousState != m_authenticated;

  if (m_authenticated) {
    refreshUserInfo();
    refreshServers();
  }

  emit tokenUpdated(authenticationStateChanged);
}

void AuthController::setUnauthenticated() {
  m_authenticated = false;
  setToken("");
}

void AuthController::refreshToken() {
  QNetworkRequest request = createNetworkRequest(REFRESH_ENDPOINT, true);
  QNetworkReply *reply = m_qnam->post(request, QByteArray());

  connect(reply, &QNetworkReply::finished, this, [this, reply]() {
    QByteArray data = reply->readAll();
    Response response = parseNetworkReply(data, *reply);
    if (!response.isOk()) {
      m_authenticated = false;
      return;
    }

    QJsonDocument document = QJsonDocument::fromJson(data);
    QString token = document.object()["token"].toString();
    setToken(token);
    m_authenticated = true;
  });

  emit loginSuccessfull();
}

void AuthController::login(const QString &login, const QString &password) {
  QJsonObject body{};
  body["login"] = login;
  body["password"] = password;

  QJsonDocument doc{body};
  QByteArray bytes = doc.toJson();

  QNetworkRequest request = createNetworkRequest(LOGIN_ENDPOINT, false, &bytes);
  QNetworkReply *reply = m_qnam->post(request, bytes);

  connect(reply, &QNetworkReply::finished, this, [this, reply]() {
    QByteArray data = reply->readAll();
    Response response = parseNetworkReply(data, *reply);
    if (!response.isOk()) {
      emit errorOccurred(*response.errors);
      return;
    }

    QJsonDocument document = QJsonDocument::fromJson(data);
    QString token = document.object()["token"].toString();
    setToken(token);
    m_authenticated = true;

    emit loginSuccessfull();
  });
}

void AuthController::recoverAccount(const QString &email) {
  QJsonObject body{};
  body["email"] = email;

  QJsonDocument doc{body};
  QByteArray bytes = doc.toJson();

  QNetworkRequest request =
      createNetworkRequest(RECOVERY_ENDPOINT, false, &bytes);
  QNetworkReply *reply = m_qnam->post(request, bytes);

  connect(reply, &QNetworkReply::finished, [this, reply]() {
    QByteArray data = reply->readAll();
    Response response = parseNetworkReply(data, *reply);
    if (!response.isOk()) {
      emit errorOccurred(*response.errors);
      return;
    }

    emit recoveryEmailSent();
  });
}

void AuthController::changePassword(const QString &currentPassword,
                                    const QString &newPassword) {
  QJsonObject body{};
  body["currentPassword"] = currentPassword;
  body["newPassword"] = newPassword;

  QJsonDocument doc{body};
  QByteArray bytes = doc.toJson();

  QNetworkRequest request =
      createNetworkRequest(PASSWORD_CHANGE_ENDPOINT, true, &bytes);
  QNetworkReply *reply = m_qnam->post(request, bytes);

  connect(reply, &QNetworkReply::finished, [this, reply]() {
    QByteArray data = reply->readAll();
    Response response = parseNetworkReply(data, *reply);
    if (!response.isOk()) {
      emit errorOccurred(*response.errors);
      return;
    }

    emit passwordChanged();
  });
}

void AuthController::changeEmail(const QString &newEmail) {
  QJsonObject body{};
  body["newEmail"] = newEmail;

  QJsonDocument doc{body};
  QByteArray bytes = doc.toJson();

  QNetworkRequest request =
      createNetworkRequest(EMAIL_CHANGE_ENDPOINT, true, &bytes);
  QNetworkReply *reply = m_qnam->post(request, bytes);

  connect(reply, &QNetworkReply::finished, [this, reply]() {
    QByteArray data = reply->readAll();
    Response response = parseNetworkReply(data, *reply);
    if (!response.isOk()) {
      if (response.statusCode == 401) {
        setUnauthenticated();
      }

      emit errorOccurred(*response.errors);
      return;
    }

    emit emailChanged();
  });
}

void AuthController::checkApiCompatibility() {
  QNetworkRequest request = createNetworkRequest(API_COMPAT_ENDPOINT, false);
  QNetworkReply *reply = m_qnam->get(request);

  connect(reply, &QNetworkReply::finished, this, [this, reply]() {
    QByteArray data = reply->readAll();
    Response response = parseNetworkReply(data, *reply);

    if (!response.isOk()) {
      emit errorOccurred(*response.errors);
      return;
    }

    QJsonDocument document = QJsonDocument::fromJson(data);
    QJsonObject object = document.object();

    uint32_t breakingHash = object["breakingHash"].toInteger();
    int minVersion = object["minVersion"].toInt();
    qDebug() << "breakingHash: " << breakingHash
             << " minVeresion: " << minVersion;

    if (minVersion != API_VERSION || breakingHash != BREAKING_HASH) {
      m_updateRequired = true;
    } else {
      m_updateRequired = false;
    }
    emit apiCompatibilityChanged();
  });
}

void AuthController::registerUser(const QString &email, const QString &username,
                                  const QString &password) {
  QJsonObject body{};
  body["email"] = email;
  body["username"] = username;
  body["password"] = password;

  QJsonDocument doc{body};
  QByteArray bytes = doc.toJson();

  QNetworkRequest request =
      createNetworkRequest(REGISTER_ENDPOINT, false, &bytes);
  QNetworkReply *reply = m_qnam->post(request, bytes);

  connect(reply, &QNetworkReply::finished, [this, reply]() {
    QByteArray data = reply->readAll();
    Response response = parseNetworkReply(data, *reply, true);
    if (!response.isOk()) {
      emit errorOccurred(*response.errors);
      return;
    }

    QJsonDocument document = QJsonDocument::fromJson(data);
    QString token = document.object()["token"].toString();
    setToken(token);
    m_authenticated = true;

    emit registerSuccessfull();
  });
}

void AuthController::logout() { setUnauthenticated(); }

void AuthController::refreshUserInfo() {
  QNetworkRequest request = createNetworkRequest(ME_ENDPOINT, true);
  QNetworkReply *reply = m_qnam->get(request);

  connect(reply, &QNetworkReply::finished, [this, reply]() {
    QByteArray data = reply->readAll();
    Response response = parseNetworkReply(data, *reply);
    if (!response.isOk()) {
      if (response.statusCode == 401) {
        setUnauthenticated();
      }

      emit errorOccurred(*response.errors);
      return;
    }

    QJsonDocument document = QJsonDocument::fromJson(data);

    QJsonObject object = document.object();
    QJsonObject userObject = object["user"].toObject();

    UserInfo info{};
    info.username = userObject["username"].toString();
    info.timeLeft = userObject["timeLeft"].toInteger();
    info.email = userObject["email"].toString();
    info.isValid = true;

    m_userInfo = info;

    emit userInfoUpdated();
  });
}

static RegionInfo parseRegionInfo(QJsonObject obj) {
  return RegionInfo{.id = obj["id"].toString(),
                    .countryCode = obj["countryCode"].toString(),
                    .countryName = obj["countryName"].toString()};
}

void AuthController::refreshServers() {
  QNetworkRequest request = createNetworkRequest(SERVERS_ENDPOINT, true);
  QNetworkReply *reply = m_qnam->get(request);

  connect(reply, &QNetworkReply::finished, this, [this, reply]() {
    QByteArray data = reply->readAll();
    Response response = parseNetworkReply(data, *reply);
    if (!response.isOk()) {
      if (response.statusCode == 401) {
        setUnauthenticated();
      }

      emit errorOccurred(*response.errors);
      return;
    }

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

void AuthController::getServerConnectionString(
    const QString &serverId, ServerStringRequest *stringRequest) {
  QJsonObject body{};
  body["port"] = 10833;

  QJsonDocument doc{body};
  QByteArray bytes = doc.toJson();

  QNetworkRequest request = createNetworkRequest(
      CONNECTION_STRING_ENDPOINT + "/" + serverId, true, &bytes);
  QNetworkReply *reply = m_qnam->post(request, bytes);

  connect(reply, &QNetworkReply::finished, stringRequest,
          [this, reply, stringRequest]() {
            QByteArray data = reply->readAll();
            Response response = parseNetworkReply(data, *reply);
            if (!response.isOk()) {
              if (response.statusCode == 401) {
                setUnauthenticated();
              }

              emit stringRequest->errorOccurred(*response.errors);
              return;
            }

            QJsonDocument document = QJsonDocument::fromJson(data);
            QString connectionUrl = document.object()["url"].toString();
            emit stringRequest->stringArrived(connectionUrl);
          });
}

void AuthController::openPaymentLink() {
  QNetworkRequest request = createNetworkRequest(PAYMENT_ENDPOINT, true);
  QNetworkReply *reply = m_qnam->get(request);

  connect(reply, &QNetworkReply::finished, this, [this, reply]() {
    QByteArray data = reply->readAll();
    Response response = parseNetworkReply(data, *reply);
    if (!response.isOk()) {
      if (response.statusCode == 401) {
        setUnauthenticated();
      }

      emit errorOccurred(*response.errors);
      return;
    }

    QJsonDocument document = QJsonDocument::fromJson(data);
    QString paymentUrl = document.object()["url"].toString();
    if (!QDesktopServices::openUrl(paymentUrl)) {
      Errors errors{};
      errors.errorMessage = tr("Payment", "Failed to open payment page");
      emit errorOccurred(errors);
    } else {
      emit paymentLinkOpened();
    }
  });
}

bool AuthController::isSpikeReady() { return !m_spike.isEmpty(); }

QString AuthController::getSpikeUrl() { return m_spike; }

QNetworkRequest AuthController::createNetworkRequest(const QString &endpoint,
                                                     bool needsAuthorization,
                                                     const QByteArray *array) {
  QNetworkRequest request(QUrl(m_spike + endpoint));
  request.setRawHeader("User-Agent", "ZloVpn");

  if (needsAuthorization) {
    request.setRawHeader("Authorization",
                         QString("Bearer " + m_token).toUtf8());
  }

  if (array) {
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("Content-Length", QByteArray::number(array->size()));
  }

  return request;
}

Response AuthController::parseNetworkReply(QByteArray &data,
                                           QNetworkReply &reply,
                                           bool formError) {
  auto httpStatus =
      reply.attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
  if (reply.error() != QNetworkReply::NoError || !Response::isOk(httpStatus)) {
    auto errors = ErrorParser::parse(data, formError);
    return Response{.statusCode = httpStatus, .errors = errors};
  }

  return Response{.statusCode = httpStatus, .errors = std::nullopt};
}