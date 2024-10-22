#ifndef ENDPOINTS_H
#define ENDPOINTS_H
#include <QString>

const QString LOGIN_ENDPOINT = "/api/v1/auth/login";
const QString REGISTER_ENDPOINT = "/api/v1/auth/register";
const QString REFRESH_ENDPOINT = "/api/v1/auth/refresh";
const QString RECOVERY_ENDPOINT = "/api/v1/auth/recover";
const QString PASSWORD_CHANGE_ENDPOINT = "/api/v1/me/password";
const QString EMAIL_CHANGE_ENDPOINT = "/api/v1/me/email";
const QString ME_ENDPOINT = "/api/v1/me";
const QString SERVERS_ENDPOINT = "/api/v1/vpn/regions";
const QString CONNECTION_STRING_ENDPOINT = "/api/v1/vpn/connect";
const QString PAYMENT_ENDPOINT = "/api/v1/bank/createPayment";

#ifdef Q_OS_MACOS
const QString MAC_UPDATE_ENDPOINT = "/api/v1/updates/mac";
#endif

#endif // ENDPOINTS_H
