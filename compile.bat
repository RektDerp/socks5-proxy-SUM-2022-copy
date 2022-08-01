::@ECHO OFF
set param=%*
if NOT "%param%"== "%param:c=%" (
rmdir /Q /S build
mkdir "build\bin\dll"
echo c
)
set /p Qtdir=Enter Qt installation root
if NOT EXIST "%Qtdir%" ( set Qtdir=C:\Qt\6.3.1\mingw_64\)
set mQtdir=%Qtdir:\=/%
if NOT "%param%"== "%param:dll=%" (
echo "dll"

xcopy %Qtdir%\bin\*.dll %CD%\build\bin\dll /S /E /I
xcopy %Qtdir%\plugins\platforms %CD%\build\bin\plugins\platforms /S /E /I
xcopy %Qtdir%\qml %CD%\build\bin\qml /S /E /I
)
cmake -S . -B build -G "MinGW Makefiles" -D Qt=Qt6 -D Qtdir=%mQtdir%
cmake --build build -j 2
"C:\Program Files (x86)\NSIS\makensis.exe" installer\installer.nsi
