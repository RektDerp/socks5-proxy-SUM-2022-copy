@ECHO OFF
if "%1"=="-c" (
rmdir /Q /S build
mkdir "build\bin\dll"
)
set /p Qtdir=Enter Qt installation root
if "%Qtdir%"=="" ( set Qtdir=C:\Qt\6.3.1\mingw_64\ )
set mQtdir=%Qtdir:\=/%
if "%1"=="-c" (
xcopy /s/e %Qtdir%\bin\*.dll build\bin\dll
xcopy /s/e %Qtdir%\plugins build\bin\plugins
xcopy /s/e %Qtdir%\qml build\bin\qml
)
cmake -S . -B build -G "MinGW Makefiles" -D Qt=Qt6 -D Qtdir=%mQtdir%
cmake --build build
"C:\Program Files (x86)\NSIS\makensis.exe" installer\installer.nsi
