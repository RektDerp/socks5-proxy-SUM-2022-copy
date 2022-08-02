::@ECHO OFF
set param=%*
if NOT "%param%"== "%param:c=%" (

echo "c"
)
if NOT "%param%"== "%param:dll=%" (
echo "dll"

)

set /p Qtdir=Enter Qt installation root
if NOT EXIST "%Qtdir%" ( set Qtdir=C:\Qt\6.3.1\mingw_64\)
set mQtdir=%Qtdir:\=/%


cmake -S . -B build -G "MinGW Makefiles" -D Qt=Qt6 -D Qtdir=%mQtdir%
cmake --build build -j 2
"C:\Program Files (x86)\NSIS\makensis.exe" installer\installer.nsi
