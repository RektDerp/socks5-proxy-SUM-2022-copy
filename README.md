# Корпоративний SOCKS5 Proxy
## Description
SOCKS is an Internet protocol used to transfer packets of data from a server to a client using an intermediate proxy server. This is the most advanced mass proxy technology available today. When using it, client traffic passes proxy server and goes to the destination point with server IP. SOCKS proxy transfers data from the client to the server and does not interfere with the content of the data itself. It is used to hide the IP address and bypass blocking.

Our PROXY server has the following functionality:
- SOCKS4, SOCKS5 support
- installer for Windows, apt installation for Linux
- launches as a service
- view, sort and filter connection statistics using the interface
- server settings: port, max. number of connections, buffer size

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
1. Download package from the repository (```socks5-proxy-2.2.8-1-any.pkg.tar.zst``` for Arch Linux, ```socks5-proxy-Qt5-2.2.8-1_amd64.deb``` for Ubuntu/Debian)
2. ```
    sudo pacman -U socks5-proxy-2.2.8-1-any.pkg.tar.zst #Arch
    sudo dpkg -i socks5-proxy-Qt5-2.2.8-1_amd64.deb 	#deb
   ```
3. To start the server: ```sudo systemctl start socks5-proxy```
4. To restart automatically on reboot: ```sudo systemctl enable socks5-proxy```
5. To close the server: ```sudo systemctl stop socks5-proxy```
6. To view statistics, select Socks5 Proxy GUI from the menu
7. To stop the server: ```sudo systemctl stop socks5-proxy```
8. Uninstallation: ```sudo apt remove socks5-proxy```

### Windows
1. Download ```socks5_installer.exe``` from the repository
2. Run and follow the instructions of the installer
3. To control the service, click on the shortcut ```Socks5 Proxy service control```
- Start - starts service
- Stop - stop service
- Enable - enable start on reboot
- Disable - disable start on reboot
4. To close the server, click the ```Stop Socks5 Proxy Service``` shortcut
5. To view statistics, click the ```Socks 5 Proxy``` shortcut or ```<installed location>\statistics\interface.exe```
6. Uninstallation: ```<installed location>\uninstall.exe```
