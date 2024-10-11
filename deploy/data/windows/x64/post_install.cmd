sc stop AmneziaWGTunnel$ZloVPN
sc delete AmneziaWGTunnel$ZloVPN
taskkill /IM "ZloVPN-service.exe" /F
taskkill /IM "ZloVPN.exe" /F
exit /b 0
