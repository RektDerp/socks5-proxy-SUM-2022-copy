# socks5-proxy-SUM-2022
Корпоративный SOCKS5 Proxy

A very interesting description

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

## Windows
1. Download latest MinGW compiler: 
    https://www.mingw-w64.org/downloads/#mingw-builds
2. Download CMake
    https://cmake.org/download/
    
Compile using script
```
    compile.bat
```

To create installer:
Download NSIS 
    https://nsis.sourceforge.io/Download
Open installer\\installer.nsi file using NSIS Compiler
![https://imgur.com/X6sa19x.png](screenshot)

---
## Usage

