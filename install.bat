
mkdir "%programfiles%\socks5-proxy-SUM-2022"
copy /y "bin\*" "%programfiles%\socks5-proxy-SUM-2022"
copy /y "config.txt" "%programfiles%\socks5-proxy-SUM-2022"
sc delete Socks5
sc create Socks5 binpath="%programfiles%\socks5-proxy-SUM-2022\service.exe"
