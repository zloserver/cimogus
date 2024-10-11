set AmneziaPath=%~dp0
echo %AmneziaPath%

"%AmneziaPath%\ZloVPN.exe" -c
timeout /t 1
sc stop ZloVPN-service
sc delete ZloVPN-service
sc stop AmneziaWGTunnel$ZloVPN
sc delete AmneziaWGTunnel$ZloVPN
taskkill /IM "ZloVPN-service.exe" /F
taskkill /IM "ZloVPN.exe" /F
exit /b 0
