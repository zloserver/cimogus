#ifndef AUTHCONTROLLER_H
#define AUTHCONTROLLER_H

#include <QObject>
#include <QNetworkAccessManager>
#include "errorParser.h"
#include "settings.h"

struct UserInfo {
    Q_GADGET

    Q_PROPERTY(QString username MEMBER username)
    Q_PROPERTY(qint64 timeLeft MEMBER timeLeft)
    Q_PROPERTY(QString localizedTimeLeft READ localizeTimeLeft)

public slots:
    QString localizeTimeLeft() const;
    qint64 monthsAvailableToAdd() const;

public:

    bool operator==(const UserInfo& other) const {
        return username == other.username && timeLeft == other.timeLeft;
    }

    bool operator!=(const UserInfo& other) const {
        return !(*this == other);
    }

    QString username;
    qint64 timeLeft;
};

struct RegionInfo {
    Q_GADGET

    Q_PROPERTY(QString id MEMBER id)
    Q_PROPERTY(QString countryCode MEMBER countryCode)
    Q_PROPERTY(QString countryName MEMBER countryName)

public:

    bool operator==(const RegionInfo& other) const {
        return id == other.id && countryCode == other.countryCode && countryName == other.countryName;
    }

    bool operator!=(const RegionInfo& other) const {
        return !(*this == other);
    }

    QString id;
    QString countryCode;
    QString countryName;
};

class ServerStringRequest : public QObject {
    Q_OBJECT

public:
    explicit ServerStringRequest(QObject *parent = nullptr) : QObject{parent} {};

signals:
    void stringArrived(const QString connectionString);
    void errorOccurred(const Errors errors);
};

class AuthController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(UserInfo userInfo MEMBER m_userInfo NOTIFY userInfoUpdated);
    Q_PROPERTY(QList<RegionInfo> regionInfo MEMBER m_regions NOTIFY regionsUpdated);
public:
    explicit AuthController(std::shared_ptr<Settings> settings, QObject *parent = nullptr);

    const QList<RegionInfo>& getRegions() const {
        return m_regions;
    }

public slots:
    bool isAuthenticated();
    QString getToken();
    void setToken(const QString& token);
    void setUnauthenticated();

    void refreshToken();
    void login(const QString& login, const QString& password);
    void registerUser(const QString& email, const QString& username, const QString& password);
    void logout();

    void refreshUserInfo();
    void refreshServers();

    QString getSelectedRegionId();
    void setSelectedRegionId(QString regionId);

    void getServerConnectionString(const QString& serverId, ServerStringRequest* stringRequest);
    void addBalance(qint32 months);

signals:
    void errorOccurred(const Errors errors);
    void errorOccurredQml(const QString errorMessage, const QVariantMap fieldErrors);
    void tokenUpdated();
    void loginSuccessfull();
    void registerSuccessfull();

    void userInfoUpdated();
    void regionsUpdated();
    void addBalanceOpened();

private:
    std::shared_ptr<Settings> m_settings;

    QString m_token{};
    UserInfo m_userInfo{};
    QList<RegionInfo> m_regions{};
    QScopedPointer<QNetworkAccessManager> m_qnam;
    bool m_authenticated{};
};

#endif // AUTHCONTROLLER_H
