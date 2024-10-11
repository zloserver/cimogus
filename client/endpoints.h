#ifndef ENDPOINTS_H
#define ENDPOINTS_H
#include <QString>

const QString API_ROOT = "http://188.245.175.39";
const QString LOGIN_ENDPOINT = "/api/v1/auth/login";
const QString REGISTER_ENDPOINT = "/api/v1/auth/register";
const QString REFRESH_ENDPOINT = "/api/v1/auth/refresh";
const QString ME_ENDPOINT = "/api/v1/me";
const QString SERVERS_ENDPOINT = "/api/v1/vpn/regions";
const QString CONNECTION_STRING_ENDPOINT = "/api/v1/vpn/connect";
const QString PAYMENT_ENDPOINT = "/api/v1/bank/createPayment";

#endif // ENDPOINTS_H
