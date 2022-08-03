# Корпоративний SOCKS5 Proxy
## Опис
SOCKS – це інтернет-протокол, який використовується для передачі пакетів із даними сервера до клієнта за допомогою проміжного проксі-сервера. На сьогодні це найбільш продвинута масова технологія для проксі. При її використанні трафік проходить через проксі-сервер, який використовує власний IP-адрес, з якого вже іде остаточне підключення до потрібної адреси. SOCKS проксі передає дані від клієнта до серверу, не втручається у вміст самих даних. Використовується з метою приховання IP адреси і обходу блокувань.

Наш PROXY сервер має такий функціонал:
- підтримка SOCKS4, SOCKS5
- інсталятор для Windows, установка apt для Linux
- запуск у вигляді сервіса
- перегляд, сортування та фільтрування статистики по підключенням за допомогою інтерфейсу
- налаштування сервера: порт, макс. кількість підключень, розмір буфера

## Building from source

### Linux 
Install dependencies: 
- Boost
- GCC 
- Qt 5
- QtQuick
- QtQuick Controls 2
- Sqlite 3

Unbuntu / Debian
```
    sudo apt update
    sudo apt install qtbase5-dev qtdeclarative5-dev libsqlite3-dev libboost-all-dev cmake build-essential qtquickcontrols2-5-dev qml-module-qtquick-controls2 qml-module-qtquick-controls git
```

Arch Linux

```
    #Qt5
    sudo pacman -Sy boost sqlite qt5-base qt5-declarative qt5-quickcontrols2 base-devel git cmake
    #Qt6
    sudo pacman -Sy boost sqlite qt6-base qt6-declarative qt6-quickcontrols2 base-devel git cmake
```

Compiling
```
    git clone https://github.com/apriorit/socks5-proxy-SUM-2022.git
    cd socks5-proxy-SUM-2022
    ./configure clean build [-D Qt=<Qt5|Qt6>] #Qt5 is default
```

Installation

`sudo ./install`

Or create package
```
    ./configure package [arch] #For arch linux
    ./configure package deb #For debian derivatives ###TODO###
```

The package file will be written to `build` directory

### Windows
1. Download latest MinGW compiler: 
    https://github.com/niXman/mingw-builds-binaries/releases/download/12.1.0-rt_v10-rev3/x86_64-12.1.0-release-posix-seh-rt_v10-rev3.7z
    
    Add \<installed location\>\bin to your system's PATH environment variable
    
2. Download CMake
    https://cmake.org/download/
    
3. Install Qt 6
    https://www.qt.io/download-qt-installer 
    
    Choose "Custom Installation"
    
    ![scr](https://imgur.com/J6C2sfd.png)
    
    And select Qt6 for MinGW 11
    
    ![](https://imgur.com/j58pYoe.png)
    
Compile using script
```
    compile.bat
```

To create installer:

1. Download NSIS: [link](https://nsis.sourceforge.io/Download)
2. Open installer\\installer.nsi file using NSIS Compiler<br>
![screenshot](https://imgur.com/X6sa19x.png)

---
## Usage
### Linux
1. Завантажити пакет з репозиторію (```socks5-proxy-2.2.8-1-any.pkg.tar.zst``` для Arch Linux, ```socks5-proxy-Qt5-2.2.8-1_amd64.deb``` для Ubuntu/Debian)
2. ```
      sudo dpkg -i socks5-proxy-Qt5-2.2.8-1_amd64.deb #deb
      sudo pacman -U socks5-proxy-2.2.8-1-any.pkg.tar.zst #Arch
   ```
4. Для запуску сервера: ```sudo systemctl start socks5-proxy```
5. Для автоматичного перезапуску при перезавантаженні: ```sudo systemctl enable socks5-proxy```
6. Для закриття сервера: ```sudo systemctl stop socks5-proxy```
7. Для перегляду статистики у меню обрати Socks5 Proxy GUI
8. Для зупинки сервера: ```sudo systemctl stop socks5-proxy```
9. Деінсталяція: ```sudo apt remove socks5-proxy```

### Windows
1. Завантажити ```socks5_installer.exe``` з репозиторію
2. Запустити та слідувати інструкціям інсталятора
3. Натиснути на ярлик ```Start Socks5 Proxy Service```
4. Для закриття сервера натиснути ярлик ```Stop Socks5 Proxy Service```
5. Для перегляду статистики натиснути ярлик ```Socks 5 Proxy```, або ```<installed location>\statistics\interface.exe```
6. Деінсталяція: ```<installed location>\uninstall.exe```
